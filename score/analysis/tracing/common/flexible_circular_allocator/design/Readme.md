# Design for Thread-Safe Circular Buffer Allocator

In our IPC tracing concept, we depend on shared memory resources to allocate memory for TMD (Trace Metadata). However, shared memory resources only provide allocation mechanisms, not deallocation. Therefore, we had to implement our custom thread-safe, lock-free Circular Buffer Allocator using atomic `compare-and-exchange` based operations.

## Operations

Six operations are available to the user:

```c
void* Allocate(const std::size_t size, const std::size_t alignment);
void Deallocate(void* const ptr, const std::size_t size);
std::size_t GetAvailableMemory();
void* GetBaseAddress();
std::size_t GetSize();
bool IsInBounds(const void* const address, const std::size_t size);
```

`Allocate` and `Deallocate` behave the same as the standard *libc* `malloc()` and `free()` routines. To be compatible with existing software, addresses returned should be aligned to 64-bytes.

## References

- [std::allocator::allocate](https://en.cppreference.com/w/cpp/memory/allocator/allocate)
- [std::allocator::deallocate](https://en.cppreference.com/w/cpp/memory/allocator/deallocate.html)

## Usage

This algorithm uses pre-allocated memory that we allocated using shared memory resources and manages it via multiple data structures which are updated atomically.

In this design, allocations and deallocations within a single chunk do not necessarily follow the same order. However, their order is preserved at the level of the chunk list. The allocator advances linearly through the buffer during allocation until it reaches the end, at which point it wraps around using circular indexing. Similarly, deallocation progresses forward through the buffer until the end is reached.

The following sequence diagram shows how allocation and deallocation sequences happen:

![Allocation and Deallocation Sequence Diagram](./score/analysis/tracing/score/static_reflection_with_serialization/flexible_circular_allocator/design/sequence_diagram.puml)

## Assumptions of Use

We assume that:

- Allocations may occur from different threads but deallocations happen from only one thread

## Data Structures Used

### List Array

The list_array_ is a fixed-size array of atomic ListEntry elements that stores the metadata of all allocated blocks. Its state and traversal are managed using the head and tail indices.

```c
std::array<std::atomic<ListEntry>, kArraySize> list_array_{};
```

### List Queue Head and Tail

- A `list_queue_head_` is an atomic uint32 to keep track of the last allocated List Entry in `list_array_` and is updated with each allocation.
- A `list_queue_tail_` is an atomic uint32 to keep track of the last deallocated List Entry in `list_array_` and is updated with each deallocation.

```c
std::atomic<uint32_t> list_queue_head_;
std::atomic<uint32_t> list_queue_tail_;
```

> **Note**: We don't encapsulate head and tail in one struct because in such a case, we would need to mark this struct as `alignas(64)` and the struct itself would not be lock-free. Therefore, it's preferred to separate both of them to ensure lock-free operation and avoid false sharing.
reference: [false sharing](https://en.wikipedia.org/wiki/False_sharing)

### ListEntry Structure

`ListEntry` is a struct which holds the metadata of a memory block: the offset in buffer_queue, the length of the block, and a flag to indicate whether the block is free or used. Each block has an associated `ListEntry` element in the `list_array_`.

```c
struct ListEntry {
    uint32_t offset;
    uint16_t length;
    uint8_t flags;
};
```

### Buffer Queue Head and Tail

- A `buffer_queue_head_` is an atomic uint32 to keep track of the last allocated address and is updated with each allocation.
- A `buffer_queue_tail_` is an atomic uint32 to keep track of the last deallocated address and is updated with each deallocation.

```c
std::atomic<uint32_t> buffer_queue_head_;
std::atomic<uint32_t> buffer_queue_tail_;
```

### BufferBlock Structure

The `BufferBlock` is at the beginning of every *block* and is considered metadata for each allocated block. Because everything between the `buffer_queue_head_` and `buffer_queue_tail_` is allocated, this forms a linked list type structure, with lengths to get to the next block instead of pointers.

```c
struct BufferBlock {
    uint32_t list_entry_offset;
    uint32_t block_length;
};
```

### Gap Address

`gap_address_` represents the address of a memory block that is too small to satisfy the current allocation request, causing the allocator to wrap around to the beginning of the buffer. During deallocation, this address is used as a reference point to determine when to wrap around as well.

```c
std::atomic<std::uint32_t> gap_address_;
```

The following diagram shows how data is managed by the above data structures:

![QOS Lockless Allocator Data Structures](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/score/static_reflection_with_serialization/flexible_circular_allocator/design/structural_view.uxf?ref=b0bbf8e033d85bba90b533030bb4d9d84b1ac780)

It is important that these data structures can be atomically updated and are ensured to be lock-free by using `is_lock_free()` to make sure each data structure is atomically updated and lock-free.

### The Buffer Queue

As the design shows, the shared memory allocated buffer is managed by head and tail. We could run into a case where there's wrap-around (tail > head), so we need to handle both cases: with wrap-around and without wrap-around. The following diagram shows how BufferQueue looks in both cases:

![Buffer Block Wrap Around vs No Wrap Around](./BufferBLock_wrap_around_no_wrap_around.png)

### Allocation

When receiving an Allocate call, we must:

- Calculate the aligned size of data + size of `BufferBlock` metadata
- Check if the remaining size in `BufferQueue` will be sufficient for the incoming allocate size
- Get a `ListEntry` element from `ListQueue` and atomically update `ListQueue` by increasing the head
- Decide if `buffer_queue_head_` will wrap around or not (if the requested size will not fit in the area between current `buffer_queue_head_` and the end of the allocated shared memory region)

#### Wrap Around Case

- Set the value for the `gap_address` to be used in the deallocate to reset `buffer_queue_tail_`
- Reset `buffer_queue_head_` to be zero to start from the beginning
- Update `buffer_queue_head_` atomically with the aligned size
- Create `BufferBlock` metadata for the block and fill it with block `length` and `list_entry_offset`
- Atomically update the corresponding `ListEntry` element in `list_array_` with `offset` and `length` and mark block as `in_use`

#### No Wrap Around Case

- Update `buffer_queue_head_` atomically with the aligned size
- Create `BufferBlock` metadata for the block and fill it with block `length` and `list_entry_offset`
- Atomically update the corresponding `ListEntry` element in `list_array_` with `offset` and `length` and mark block as `in_use`

### Deallocation

- Cast the sent address to get `BufferBlock` metadata
- Validate that the requested address is in the correct bounds
- Reset `buffer_queue_tail_` if it is equal to the calculated `gap_address_`
- Set `ListEntry` element as `free`
- Start freeing the blocks if the requested address to deallocate is equal to `buffer_queue_tail_` since some blocks were marked free but are not actually freed. This is because the deallocation is not in the same order as allocation. It will also start freeing when `buffer_queue_tail_` is zero since it was wrapped around.
- Iterate on all the blocks marked as `free` blocks by getting `offset` and `length` of each block by doing pointer arithmetic
- If `gap_address_` was reached while iterating the blocks, we shall reset `buffer_queue_tail_` to zero
- Free all blocks marked as `free` and update `buffer_queue_tail_` with the freed block size
- Reset block `ListEntry` element
- `ListQueue.tail` is updated
