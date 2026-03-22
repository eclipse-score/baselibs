# score::shm pointer-policy experiment TODO

## Context

This branch validates performance and architecture boundaries by comparing
explicit pointer policies:

- `ShmPointerPolicy` (`OffsetPtr` / `NullableOffsetPtr`)
- `ShmDirectPointerPolicy` (`DirectPtr` for both aliases)

The branch now also injects pointer wrappers into container internals via
`PointerPolicy`:

- `score::shm::Map<..., PointerPolicy = ShmPointerPolicy>`
- `score::shm::Vector<..., PointerPolicy = ShmPointerPolicy>`

This keeps default behavior unchanged while enabling controlled experiments with
alternative pointer wrappers.

## Next optimization tasks

- [ ] Build a focused microbenchmark for map internals (insert/rebalance/find/erase paths) to compare
      `std::map` vs `score::shm::Map` at equal pointer cost.
- [ ] Profile `score::shm::Map` random-build and random-lookup hot paths (branching, rotations, node access)
      and reduce instruction count/branch mispredictions.
- [ ] Evaluate node layout and allocator interaction in `score::shm::Map` for cache locality improvements.
- [ ] Consider extracting a reusable tree core that is parameterized by `PointerPolicy` and allocator model.
- [ ] Define a minimal pointer-wrapper concept contract for policies (`Ptr<T>`, `NullablePtr<T>`, `get()`,
      copy/assign semantics) and enforce it with compile-time checks.
- [ ] Extend benchmarks to run matrix comparisons across pointer policies (offset vs direct wrappers)
      for both vector and map families.
