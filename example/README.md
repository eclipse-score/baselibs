# `score::shm` Shared Memory Containers Example

Demonstrates cross-process data sharing using `score::shm::Vector` and `score::shm::Map` in POSIX shared memory.

## Design

`BoundedContainers<T, N, M>` bundles:

- a `score::shm::Vector<T>` with inline fixed-capacity storage (`CreateWithBuffer()`),
- a `score::shm::Map<int, int>` allocated from an inline `MonotonicBufferResource`.

All backing storage lives inside the same shared-memory object, so `OffsetPtr` state
resolves correctly when mapped at different base addresses in parent/child processes.

## How it works

1. **Parent** (`shm_parent.cpp`): Creates a POSIX shared memory region, placement-constructs
   a `BoundedContainers<int, ...>`, writes vector/map data, then launches the child process.
2. **Child** (`shm_child.cpp`): Maps the same SHM region (at a different virtual address),
   reads both containers, appends/sorts vector elements, and updates map values.
3. **Parent** observes the child's modifications after it exits.

Synchronization between parent and child is external to `BoundedContainers` (the parent
`waitpid`s for the child).

## Building and running

```bash
bazel build //example:shm_parent //example:shm_child
bazel-bin/example/shm_parent
```
