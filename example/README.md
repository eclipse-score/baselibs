# `score::shm::Vector` Shared Memory Example

Demonstrates cross-process data sharing using `score::shm::Vector` in POSIX shared memory.

## Design

`BoundedVector<T, N>` bundles a `score::shm::Vector<T>` with an inline byte buffer:

- The Vector is created via `CreateWithBuffer()`, operating in fixed-capacity buffer mode.
  No allocator or memory resource is involved at runtime — all storage is the embedded
  `std::byte buffer[N * sizeof(T)]` within the struct.
- Because both the Vector metadata and element storage live at fixed offsets within the
  struct, `OffsetPtr` resolves correctly when the struct is mapped at different base
  addresses in different processes.
- The allocator member in the Vector exists (type is unchanged) but is unused.

## How it works

1. **Parent** (`shm_parent.cpp`): Creates a POSIX shared memory region, placement-constructs
   a `BoundedVector<int, 1024>`, writes elements, then launches the child process.
2. **Child** (`shm_child.cpp`): Maps the same SHM region (at a different virtual address),
   reads and appends elements, sorts the vector.
3. **Parent** observes the child's modifications after it exits.

Synchronization between parent and child is external to `BoundedVector` (the parent
`waitpid`s for the child).

## Building and running

```bash
bazel build //example:shm_parent //example:shm_child
bazel-bin/example/shm_parent
```

