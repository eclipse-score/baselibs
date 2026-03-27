# score/memory/shared — Deprecated

**All targets in this package are deprecated and frozen.**

The maintained version of `score/memory/shared` now lives in the
[communication repo](https://github.com/eclipse-score/communication)
under the same path.

## Why this exists

`score/memory/shared` was moved to the communication repo because that's
where its main consumer (the LoLa IPC binding) lives. Having it there
lets us iterate on the shared memory layer without going through the
baselibs release cycle.

We can't just delete it from baselibs because some packages here still
depend on it (tracing, containers tests). And we can't make those
packages point to the communication copy because that would be a
circular dependency in bzlmod — communication already depends on
baselibs.

## Why the :types target was narrowed

The `:types` target used to be a grab-bag aggregate that bundled 10+
sub-targets (offset_ptr, map, string, vector, registry, etc.). Consumers
that only needed one or two of these were pulling in all of them.

This is a problem because if a communication binary uses
`@score_baselibs//score/containers:dynamic_array` which transitively
depends on `@score_baselibs//score/memory/shared:types`, both the
baselibs copy AND the communication copy of memory/shared end up linked
in the same binary — duplicate symbols.

We replaced `:types` deps with the specific targets each consumer
actually uses:
- `chunk_list` only needs `managed_memory_resource` and `vector`
- `flexible_circular_allocator` only needs `managed_memory_resource` and `offset_ptr`
- container tests only need `managed_memory_resource`, `offset_ptr`, and `polymorphic_offset_ptr_allocator`

## Warning: do NOT write new code against these targets

These targets are frozen — they exist only for backward compatibility
with existing consumers (tracing, containers tests).

Do not write new code against these targets. Use the communication
copy for any new work.

## What to do if you depend on these targets

Update your `BUILD` deps from:
```
@score_baselibs//score/memory/shared:<target>
```
to:
```
@score_communication//score/memory/shared:<target>
```

The APIs are identical — it's the same code, just maintained in a
different repo now.

## Timeline

1. **Now**: all targets deprecated, build warnings shown
2. **When tracing refactors their interface**: primitive targets removed
3. **After deprecation period**: remaining targets removed
4. **End state**: this directory deleted from baselibs
