# Justification why the reinterpret_cast is acceptable

## Claim

`flatbuffers::CastToArray` / `CastToArrayOfEnum` (in `flatbuffers/array.h`)
`reinterpret_cast` a raw `T[length]` into a `flatbuffers::Array<T, length>`. This
violates MISRA C++:2023 Rule 8.2.5 (use of `reinterpret_cast` to an unrelated class type).

This document is the evidence for that claim.

## How the evidence is produced

The schema [`array_cast_fixture.fbs`](array_cast_fixture.fbs) exercises every
fixed-array case — a scalar array, an enum scalar, an enum array and a 64-bit
scalar array. `flatc` compiles it into a `NestedStruct` whose raw C arrays are
`private` and whose constructor and accessors route through `CastToArray` /
`CastToArrayOfEnum`. That generated header is the safe, typed API the user sees.

The end-to-end evidence is [`flatbuffers_array_test.cpp`](flatbuffers_array_test.cpp)
(`ArrayCastSafetyGeneratedTest`), which builds a serialized buffer through
the generated API and reads every field back through the reinterpret-cast view,
verifying each measure below.

## Why this substantiates "measures for correct usage"

The MISRA-flagged `reinterpret_cast` is never in the user's hands. The generated
header enforces:

1. **Encapsulation** — the raw `int32_t a_[2]` storage is `private`; the only
   surface is the accessors. Users never call `CastToArray` themselves.
2. **Const-correctness** — read accessors return `const Array<T, N>*`, so the
   aliased buffer cannot be mutated through them.
3. **Type safety for enums** — enum arrays route through `CastToArrayOfEnum<E>`,
   whose `static_assert(sizeof(E) == sizeof(T), "invalid enum type E")`
   (`array.h`) makes a mismatched storage type a compile error.
4. **Size safety for writes** — mutation is only via `CopyFromSpan(span<const T, N>)`.
   The span's extent `N` is a compile-time template parameter matching the field
   length, so a wrong-length write does not compile.
5. **No uninitialized reads** — padding fields are explicitly zero-initialized in
   every constructor.

## Conclusion

The `reinterpret_cast` in `CastToArray` / `CastToArrayOfEnum` is an internal
implementation detail of `array.h`. The `flatc`-generated header is the safe,
typed, const-correct API the user sees.

Correctness of the reinterpret-cast view against a generated buffer is
checked end-to-end by `ArrayCastSafetyGeneratedTest` in
[`flatbuffers_array_test.cpp`](flatbuffers_array_test.cpp). Users of other
generated schemas must provide the equivalent test against their own buffers.
