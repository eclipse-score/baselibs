# `score::nothrow` Shared Memory Containers Example

Demonstrates cross-process data sharing — including **mutation from both sides** —
using `score::nothrow::Vector` and `score::nothrow::Map` in POSIX shared memory.

## Design

`BoundedContainers<T, VectorCapacity, MapNodeCount>` bundles:

- a `score::nothrow::Vector<T>` with inline fixed-capacity storage (`CreateWithBuffer()`),
- a `score::nothrow::Map<int, int>` whose nodes are carved from an inline buffer by an
  internal free-list pool, also via `CreateWithBuffer()`.

Neither container uses an allocator or memory resource. That matters for shared memory:
a `std::pmr`-style resource is polymorphic (its **vtable pointer** is process-local), holds
a **raw buffer pointer**, and points at a **process-local upstream singleton** — none of
which survive being mapped at a different base address in another process. By provisioning
both containers directly from inline buffers, the bundle has none of those: all backing
storage lives inside the same shared-memory object, and every internal pointer is stored
through the default offset pointer policy (`OffsetSlot`) as a self-relative offset. The
whole bundle is therefore position-independent and resolves correctly at any mapping
address.

Because the map manages its own free-list pool over the inline buffer, it is bounded to
`MapNodeCount` live entries and reuses node storage on erase.

## How it works

1. **Parent** (`shm_parent.cpp`): creates a POSIX shared memory region sized to
   `sizeof(ShmType)`, placement-constructs a `BoundedContainers<int, ...>`, writes
   vector/map data, then `fork`/`execl`s the child binary (passed as `argv[1]`).
2. **Child** (`shm_child.cpp`): maps the same region — at a *different* virtual address —
   reads both containers, appends and sorts vector elements, **updates existing map values
   in place**, and **inserts new map entries**.
3. **Parent** observes the child's modifications after it exits.

Cross-process *map mutation* is the key capability here. The child both updates and grows a
`Map` that the parent constructed, in a different address space, and the parent sees the
result. The inserts are the stronger case: each one allocates a fresh node from the map's
free-list pool, which lives inside the shared segment — so the child performs **node
allocation that the parent then reads** (the map grows from 3 to 5 entries in the demo).
This is only possible because there is no allocator/resource object to relocate — the map's
node links, pool base, and free-list head are all offsets or indices, valid at any mapping
address.

> **Note:** position-independence is not concurrency safety. This example shares the
> segment *sequentially* — the parent `waitpid`s for the child, so only one process touches
> the containers at a time. Concurrent writers would need synchronization (e.g. a mutex)
> placed inside the shared segment; that is external to `BoundedContainers`.

## Building and running

```bash
bazel build //example:shm_parent //example:shm_child
# The parent forks and execs the child binary given as its first argument:
bazel-bin/example/shm_parent bazel-bin/example/shm_child
```

## Memory resource scopes (`resource_scopes.cpp`)

A single-process companion example demonstrating the failure-handling decision
rule (`docs/nothrow/architecture/failure_handling.rst`): the same operation —
allocation — is handled differently depending on where the memory budget lives.

- **Co-located budget:** a local buffer and `MonotonicBufferResource` in the
  same scope as the consuming container. The budget is visible two lines above
  its use, so allocation failure could only be a bug — the `OrAbort` variants
  are correct and trap at the defect site.
- **Dislocated budget:** `main()` configures the process-wide default resource
  centrally (standing in for integration-time configuration); business logic
  (`BuildSquares()`) knows nothing about the sizing and returns
  `score::Result`. An oversized — possibly attacker-sized — request is
  rejected as a contained error and the process keeps serving.

```bash
bazel run //example:resource_scopes
```
