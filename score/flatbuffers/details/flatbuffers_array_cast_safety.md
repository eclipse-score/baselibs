# Justification why the reinterpret_cast is acceptable

## Claim

`flatbuffers::CastToArray` / `CastToArrayOfEnum` (in `flatbuffers/array.h`)
`reinterpret_cast` a raw `T[length]` into a `flatbuffers::Array<T, length>`. This
violates MISRA C++:2023 Rule 8.2.5 (use of `reinterpret_cast` to an unrelated class type).

This document is the evidence for that claim.

## How the evidence was produced

1. Built the compiler from the vendored FlatBuffers dependency:
   `bazel build @flatbuffers//:flatc --copt=-Wno-error`
   (`-Wno-error` only because flatc's own sources trip this repo's `-Werror`.)
2. Ran it on the upstream schema that exercises every fixed-array case —
   scalar arrays, enum arrays and struct arrays — `tests/arrays_test.fbs`:
   ```
   bazel-bin/external/flatbuffers+/flatc --cpp --scoped-enums -o . arrays_test.fbs
   ```

### Schema (`arrays_test.fbs`, excerpt)

```
namespace MyGame.Example;

enum TestEnum : byte { A, B, C }

struct NestedStruct{
  a:[int:2];
  b:TestEnum;
  c:[TestEnum:2];
  d:[int64:2];
}
```

### Generated C++ (`arrays_test_generated.h`, `NestedStruct`)

```cpp
FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(8) NestedStruct FLATBUFFERS_FINAL_CLASS {
 private:
  int32_t a_[2];          // <-- raw C arrays are PRIVATE
  int8_t  b_;
  int8_t  c_[2];
  int8_t  padding0__;  int32_t padding1__;
  int64_t d_[2];

 public:
  // Constructor: the ONLY write path is CopyFromSpan with a fixed-extent span.
  NestedStruct(::flatbuffers::span<const int32_t, 2> _a,
               MyGame::Example::TestEnum _b,
               ::flatbuffers::span<const MyGame::Example::TestEnum, 2> _c,
               ::flatbuffers::span<const int64_t, 2> _d)
      : b_(::flatbuffers::EndianScalar(static_cast<int8_t>(_b))),
        padding0__(0), padding1__(0) {
    ::flatbuffers::CastToArray(a_).CopyFromSpan(_a);
    ::flatbuffers::CastToArrayOfEnum<MyGame::Example::TestEnum>(c_).CopyFromSpan(_c);
    ::flatbuffers::CastToArray(d_).CopyFromSpan(_d);
  }

  // Read accessors: internal cast, returning a typed, CONST Array<T,N>*.
  const ::flatbuffers::Array<int32_t, 2> *a() const {
    return &::flatbuffers::CastToArray(a_);
  }
  MyGame::Example::TestEnum b() const {
    return static_cast<MyGame::Example::TestEnum>(::flatbuffers::EndianScalar(b_));
  }
  const ::flatbuffers::Array<MyGame::Example::TestEnum, 2> *c() const {
    return &::flatbuffers::CastToArrayOfEnum<MyGame::Example::TestEnum>(c_);
  }
  const ::flatbuffers::Array<int64_t, 2> *d() const {
    return &::flatbuffers::CastToArray(d_);
  }
};
```

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

Users must still provide an end-to-end test checking correctness of the
reinterpret-cast view against their own generated buffers.
