# shm

`score::shm` provides a small shared-memory-oriented toolkit that intentionally mirrors familiar C++ interfaces where possible:

- `score::shm::MemoryResource` / `score::shm::PolymorphicAllocator<T>` follow the overall model of `std::pmr::memory_resource` and `std::pmr::polymorphic_allocator`. Their primary purpose is providing a **nothrow allocation interface** that returns `nullptr` on failure, which is the contract expected by `score::shm` containers.
- `score::shm::OffsetPtr<T>` / `score::shm::NullableOffsetPtr<T>` provide relative-pointer storage semantics.

This README focuses on **deviations and guarantees**, not on re-documenting standard APIs.

## MemoryResource and PolymorphicAllocator

Standard `std::pmr` allocation throws `std::bad_alloc` on failure. `score::shm` containers cannot use exceptions — they propagate allocation failures as `score::Result` errors. To bridge this gap, `MemoryResource::allocate()` and `PolymorphicAllocator::allocate()` are fully `noexcept` and return `nullptr` when allocation fails.

Additional deviations from `std::pmr`:

- Default resource is `GetDefaultResource()` (initially backed by `GetNewDeleteResource()`). Can be changed via `SetDefaultResource()`.
- `GetNullMemoryResource()` provides a resource that always fails allocation (returns `nullptr`). Useful for testing container error paths.
- The bound resource is stored as a raw pointer (process-local; not placed in shared memory).

### MonotonicBufferResource

`score::shm::MonotonicBufferResource` follows the `std::pmr::monotonic_buffer_resource` model:

- Allocates from a user-provided buffer by advancing a bump pointer.
- On buffer exhaustion, delegates to an upstream resource (defaults to `GetDefaultResource()`).
- Individual `deallocate()` calls are no-ops; use `release()` to reclaim the full buffer.
- All pointer state uses raw pointers (process-local; not placed in shared memory).

## OffsetPtr design focus

`score::shm::OffsetPtr` is designed around a strict round-trip invariant:

1. On construction, state is captured using integer arithmetic derived from the provided raw pointer and `this`.
2. On conversion back to `T*` / `T const*`, the original pointer value is reconstructed by inverse integer arithmetic.
3. Copy/move/assignment transfer state via the original raw pointer value, then rebase relative to the target object location.

### Why this matters

The implementation is intentionally based on the standard-blessed pointer round-trip pattern (`pointer -> integer -> same pointer`) and on explicit arithmetic invariants within one process address space.

This is a substantial design difference compared to many fancy-pointer implementations (e.g. `boost::interprocess::offset_ptr`) and also differs from the design goals/trade-offs of `score::memory::shared::OffsetPtr`.

## Scope

`score::shm` currently targets header-only building blocks and keeps the API surface small. When in doubt, prefer standard library behavior and treat `score::shm` as a specialization layer for shared-memory-safe pointer/resource representation.

## Containers

The general pattern of containers planned in `score::shm` is:

- Functional clones of the standard counterparts.
- All member functions are `noexcept`.
- Member functions that are non-throwing in the standard interface keep the standard spelling and signature as closely as possible to preserve named interface requirements.
- Bounds checking member functions, e.g. `at()`, `back()`, `front()` keep the standard spelling and signature, plus noexcept, and shall abort in case of failure. The user can check these preconditions easily in advance and does not depend on doing a post-invocation check of function result.
- Member functions that are potentially throwing `std::bad_alloc` in the standard interface (most size-changing operations) cannot be handled by users regarding memory preconditions: There is no API to query available memory; container growth strategy is an implementation detail out of user control; There are no atomic test-and-allocate operations defined by the C++ standard. Therefore they are provided in two variants with the same parameter list:
  - An error-propagating variant with PascalCase naming, returning `score::Result<T>` or `score::ResultBlank`.
  - An aborting variant with PascalCase naming and `OrAbort` suffix, returning the original value category and aborting on failure. These shall be used when an out-of-memory situation is algorithmically impossible, for example because the user prepared the memory resource in the same scope as using the container. Returning results would require a user to implement untestable error handling. Use of these functions implies asserting that memory is sufficient, and failure would be correctly classified as a bug and result in abort.
- Member functions provided as error handling derivatives described above are not provided with their standard library signature.
- Pointer members in container state are persisted using `score::shm::OffsetPtr` or `score::shm::NullableOffsetPtr` as needed.
- Pointer parameters, type aliases, and local variables follow the standard/raw-pointer style where possible; wrapping/unwrapping is used only for persisted container state.
- All containers use `score::shm::PolymorphicAllocator` as default allocator.
- Error propagation uses `score::shm::ContainerErrorCode` and `MakeError(...)`.
- Fallible construction is exposed as static factory methods:
  - `Create(...)` returns `score::Result<Container>`
  - `CreateOrAbort(...)` delegates to `Create(...)` and aborts on error

Example naming shape:

```cpp
// capacity-only construction (no elements)
static score::Result<Vector> CreateWithCapacity(size_type capacity,
                                                allocator_type allocator = allocator_type{}) noexcept;

// std::vector(count) / std::vector(count, value) equivalents
static score::Result<Vector> Create(size_type count,
                                    allocator_type allocator = allocator_type{}) noexcept;
static score::Result<Vector> Create(size_type count, const value_type& value,
                                    allocator_type allocator = allocator_type{}) noexcept;

// all Create variants have OrAbort counterparts
static Vector CreateOrAbort(size_type count,
                            allocator_type allocator = allocator_type{}) noexcept;

// potentially throwing std-like operation -> error-propagating variant
score::ResultBlank PushBack(const value_type& value) noexcept;

// same operation -> aborting convenience variant
void PushBackOrAbort(const value_type& value) noexcept;

// bounds-checked access — aborts on precondition violation
reference at(size_type index) noexcept;
reference front() noexcept;
reference back() noexcept;
```

### `score::shm::Vector`

Use `score::shm::OffsetPtr` as storage pointer. No allocated memory is represented by `capacity == 0` rather than `nullptr`.
