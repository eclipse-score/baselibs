# score::nothrow pointer-box experiment TODO

## Context

This branch validates performance and architecture boundaries by comparing
explicit pointer policies:

- `OffsetSlotPolicy` (`OffsetSlot` / `NullableOffsetSlot`)
- `RawSlotPolicy` (`RawSlot` for both aliases)

The branch now also injects pointer slots into container internals via
`PointerPolicy`:

- `score::nothrow::Map<..., PointerPolicy = OffsetSlotPolicy>`
- `score::nothrow::Vector<..., PointerPolicy = OffsetSlotPolicy>`

This keeps default behavior unchanged while enabling controlled experiments with
alternative pointer slots.

## Next optimization tasks

- [x] Build a focused microbenchmark for map internals (insert/rebalance/find/erase paths) to compare
      `std::map` vs `score::nothrow::Map` at equal pointer cost.
- [x] Profile `score::nothrow::Map` random-build and random-lookup hot paths (branching, rotations, node access)
      and reduce instruction count/branch mispredictions.
- [ ] Evaluate node layout and allocator interaction in `score::nothrow::Map` for cache locality improvements.
- [ ] Consider extracting a reusable tree core that is parameterized by `PointerPolicy` and allocator model.
- [ ] Define a minimal pointer-box concept contract for policies (`Ptr<T>`, `NullablePtr<T>`, `get()`,
      copy/assign semantics) and enforce it with compile-time checks.
- [ ] Extend benchmarks to run matrix comparisons across pointer policies (offset vs raw boxes)
      for both vector and map families.
