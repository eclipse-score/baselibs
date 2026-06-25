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
   reads both containers, appends and sorts vector elements, and **updates map values
   in place**.
3. **Parent** observes the child's modifications after it exits.

Cross-process *map mutation* is the key capability here: the child writes into a `Map`
that the parent constructed, in a different address space, and the parent sees the result.
This is only possible because there is no allocator/resource object to relocate — the map's
node links, pool base, and free-list head are all offsets or indices.

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
