# OffsetBox Benchmark

## Overview

Measures the operational overhead of `OffsetBox<T>` pointer semantics (offset arithmetic,
bounds checking) for:

- nested `Vector<Vector<int>>` random access, compared to raw `T*` pointers in `std::vector`
- map workloads (`std::map`, `score::memory::shared::Map`, `score::nothrow::Map`) covering
  random-order build, randomized key lookup and begin→end iteration

The benchmark uses a 1000×1000 nested vector with 100,000 random `(i, j)` element accesses.
The random access pattern prevents the CPU from caching resolved OffsetBox base addresses
in registers, exercising the full offset-arithmetic path on every dereference.

## Building and Running

**Build (optimized — use this for meaningful results):**

```bash
bazel build -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_box_benchmark
```

> **Note:** The `--per_file_copt` flag relaxes strict compiler warnings for external
> dependencies (Google Benchmark). This is needed because the project toolchain enables
> `-Werror` globally, which causes build failures in third-party code.

**Run:**

```bash
bazel-bin/score/nothrow/pointer_box_benchmark
```

Or build and run in one step:

```bash
bazel run -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_box_benchmark
```

**Run only vector + map family comparisons:**

```bash
bazel run -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_box_benchmark -- \
  --benchmark_filter='Std(Vector|Map)|MemoryShared(Vector|Map)|Shm(Direct|Reloc)(Vector|Map)' \
  --benchmark_min_time=0.05s
```

### With `-O3`

```bash
bazel build -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  --per_file_copt='.*@-O3,-Wno-error=maybe-uninitialized' \
  //score/nothrow:pointer_box_benchmark
```

## Benchmark Variants

Naming convention used in this benchmark:

- `Std*`: standard library baseline (`std::vector`, `std::map`)
- `MemoryShared*`: `score::memory::shared::*` implementation family
- `RawBox*`: `score::nothrow::*` with direct-pointer policy (`RawBoxPolicy`)
- `ShmReloc*`: `score::nothrow::*` with relocatable offset-pointer policy

| Benchmark | What it measures |
|---|---|
| `BM_StdVector_RandomAccess` | Baseline: `std::vector<std::vector<int>>` with raw `T*` pointers |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | `score::memory::shared::Vector` OffsetBox offset-arithmetic overhead only (bounds check bypassed) |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | `score::memory::shared::Vector` OffsetBox offset arithmetic + bounds validation |
| `BM_RawBoxVector_RandomAccess` | `score::nothrow::Vector` with direct-pointer policy (`RawBoxPolicy`) |
| `BM_ShmRelocVector_RandomAccess` | `score::nothrow::Vector` with relocatable offset-pointer policy |
| `BM_StdMap_RandomBuild` | Baseline: `std::map<int,int>` random-order insertion |
| `BM_MemorySharedMap_RandomBuild` | `score::memory::shared::Map<int,int>` random-order insertion |
| `BM_ShmDirectMap_RandomBuild` | `score::nothrow::Map<int,int>` with direct-pointer policy |
| `BM_ShmRelocMap_RandomBuild` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy |
| `BM_StdMap_RandomAccess` | Baseline: `std::map<int,int>` randomized key lookup |
| `BM_MemorySharedMap_RandomAccess` | `score::memory::shared::Map<int,int>` randomized key lookup |
| `BM_ShmDirectMap_RandomAccess` | `score::nothrow::Map<int,int>` with direct-pointer policy randomized key lookup |
| `BM_ShmRelocMap_RandomAccess` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy randomized key lookup |
| `BM_StdMap_Iterate` | Baseline: `std::map<int,int>` begin→end iteration |
| `BM_MemorySharedMap_Iterate` | `score::memory::shared::Map<int,int>` begin→end iteration |
| `BM_ShmDirectMap_Iterate` | `score::nothrow::Map<int,int>` with direct-pointer policy begin→end iteration |
| `BM_ShmRelocMap_Iterate` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy begin→end iteration |
| `BM_StdMap_Internal_InsertRebalance` | Focused std::map insert/rebalance path using ascending insert order |
| `BM_ShmDirectMap_Internal_InsertRebalance` | Focused score::nothrow::Map insert/rebalance path using ascending insert order |
| `BM_StdMap_Internal_Find` | Focused std::map find hot path over randomized successful lookups |
| `BM_ShmDirectMap_Internal_Find` | Focused score::nothrow::Map find hot path over randomized successful lookups |
| `BM_StdMap_Internal_EraseRebalance` | Focused std::map erase/rebalance path using randomized erase order |
| `BM_ShmDirectMap_Internal_EraseRebalance` | Focused score::nothrow::Map erase/rebalance path using randomized erase order |

## Results

Measured on host `marlin` with benchmark context:

- 16 logical CPUs @ ~5135 MHz
- L3 cache: 16 MiB
- `cpu_scaling_enabled: true`
- build: `bazel run -c opt //score/nothrow:pointer_box_benchmark`
- runtime flags: `--benchmark_min_time=0.3s --benchmark_repetitions=3 --benchmark_report_aggregates_only=true`

Element type: `int` (4 bytes) in `Vector<Vector<int>>` with 1000×1000 elements and 100,000 random accesses.

### Full benchmark suite (mean CPU time, ns)

| Benchmark | Mean CPU (ns) |
|---|---:|
| `BM_StdVector_RandomAccess` | 132,983 |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | 20,519,648 |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | 22,451,792 |
| `BM_RawBoxVector_RandomAccess` | 137,778 |
| `BM_ShmRelocVector_RandomAccess` | 150,015 |
| `BM_StdMap_RandomBuild` | 849,877 |
| `BM_MemorySharedMap_RandomBuild` | 83,105,281 |
| `BM_ShmDirectMap_RandomBuild` | 926,174 |
| `BM_ShmRelocMap_RandomBuild` | 1,164,263 |
| `BM_StdMap_RandomAccess` | 3,479,497 |
| `BM_MemorySharedMap_RandomAccess` | 397,362,142 |
| `BM_ShmDirectMap_RandomAccess` | 3,307,379 |
| `BM_ShmRelocMap_RandomAccess` | 7,136,843 |
| `BM_StdMap_Iterate` | 49,237 |
| `BM_MemorySharedMap_Iterate` | 7,259,313 |
| `BM_ShmDirectMap_Iterate` | 48,785 |
| `BM_ShmRelocMap_Iterate` | 73,838 |
| `BM_StdMap_Internal_InsertRebalance` | 182,413 |
| `BM_ShmDirectMap_Internal_InsertRebalance` | 117,100 |
| `BM_StdMap_Internal_Find` | 2,956,408 |
| `BM_ShmDirectMap_Internal_Find` | 2,848,400 |
| `BM_StdMap_Internal_EraseRebalance` | 506,597 |
| `BM_ShmDirectMap_Internal_EraseRebalance` | 593,704 |

### Ratio summary (vs std baseline in each family)

| Comparison | Ratio |
|---|---:|
| `ShmDirectVector_RandomAccess / StdVector_RandomAccess` | 1.04× |
| `ShmRelocVector_RandomAccess / StdVector_RandomAccess` | 1.13× |
| `MemorySharedVector_NoBoundsCheck / StdVector_RandomAccess` | 154.30× |
| `MemorySharedVector_BoundsChecked / StdVector_RandomAccess` | 168.83× |
| `ShmDirectMap_RandomBuild / StdMap_RandomBuild` | 1.09× |
| `ShmDirectMap_RandomAccess / StdMap_RandomAccess` | 0.95× |
| `ShmDirectMap_Iterate / StdMap_Iterate` | 0.99× |
| `ShmDirectMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 0.64× |
| `ShmDirectMap_Internal_Find / StdMap_Internal_Find` | 0.96× |
| `ShmDirectMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 1.17× |

### Current takeaways

- `ShmDirect` is now near/at parity for map lookup and iteration, and faster in focused insert/rebalance and find microbenchmarks.
- `ShmDirectMap` random build remains slightly slower than `std::map`.
- `ShmDirectMap` erase/rebalance remains slower than `std::map` and is the primary remaining gap.
- `score::memory::shared::*` remains far slower in this benchmark, especially for random map access and vector random access.

### Cross-validation with independent implementation

An independent OffsetBox Vector implementation
([TrivialityInversion](https://github.com/jmd-emermern/TrivialityInversion), same
`intptr_t`-based inline approach) shows comparable results on the same hardware using
CMake Release builds (`-O3`):

| Element type | `std::vector` | OffsetBox Vector | Overhead |
|---|---|---|---|
| `uint8_t` | 1.10 ms | 1.24 ms | ~12% |
| `uint32_t` | 1.38 ms | 1.49 ms | ~8% |
| `uint64_t` | 3.13 ms | 3.35 ms | ~7% |

The overhead varies by element size (smaller elements → higher relative overhead, likely
due to the constant per-access OffsetBox conversion cost being larger relative to the
cheaper cache-line loads for small elements).

### Summary

| Configuration | Inline OffsetBox overhead | Cross-TU OffsetBox overhead |
|---|---|---|
| Bazel `-O2` (default) | ~15% | ~180× |
| CMake `-O3` (cross-validation) | ~7–12% | n/a |

**Key findings:**

- **The existing `score::memory::shared` OffsetBox has ~180× overhead** on random access,
  dominated by cross-translation-unit call barriers in `pointer_arithmetic_util.cpp` and
  the `uintptr_t` round-trip pattern that destroys pointer provenance for the optimizer.
- **A header-inline OffsetBox reduces overhead to ~15%** vs raw pointers. The `intptr_t`-based
  offset computation is fully visible to the compiler, allowing it to optimize through the
  pointer reconstruction. This confirms that the overhead is not inherent to relative pointers
  but caused by the implementation strategy.
- **Bounds checking adds ~10%** on top of the existing implementation's offset arithmetic
  cost. In the inline implementation, this cost would be proportionally smaller.
- **This is a worst-case benchmark** — random access with double indirection through nested
  containers. Sequential access patterns would show lower overhead due to CPU prefetching.

## Analysis: Root Cause of score::memory::shared::OffsetPtr Overhead

The ~180× overhead is not inherent to the concept of relative pointers. It is caused by the
implementation strategy chosen to avoid undefined behavior in pointer arithmetic.

### The problem

`OffsetPtr<T>` stores a byte offset from its own address to the target object. To reconstruct
the target pointer, it must compute `this_address + offset`. However, direct pointer arithmetic
(`ptr + n`) is only well-defined within an array or one past its end (C++ [expr.add]). The
OffsetBox and its target are in different allocations, so direct pointer arithmetic would be UB.

### The chosen solution: cast through `uintptr_t`

Every pointer reconstruction (dereference, copy, comparison) goes through this pattern in
`pointer_arithmetic_util`:

```cpp
uintptr_t CastPointerToInteger(const void* ptr) {
    return reinterpret_cast<std::uintptr_t>(ptr);
}
// ... integer arithmetic ...
T* CastIntegerToPointer(uintptr_t integer) {
    return reinterpret_cast<T*>(integer);
}
```

The call chain for every `operator*()`, `operator->()`, `operator[]()` is:

```
GetPointerWithBoundsCheck → CalculatePointerFromOffset → AddOffsetToPointer
  → CastPointerToInteger → integer add → CastIntegerToPointer
```

### Why this is expensive

1. **Cross-translation-unit call barrier.** `CastPointerToInteger` is defined in
   `pointer_arithmetic_util.cpp`, not in the header. Without LTO, the compiler cannot inline
   it, so every dereference involves an actual function call. This alone likely accounts for
   the majority of the overhead.

2. **Pointer provenance is destroyed.** Even when inlined (e.g. with LTO), the
   `reinterpret_cast` round-trip through `uintptr_t` breaks GCC's pointer provenance tracking.
   The optimizer can no longer determine which allocation the pointer belongs to, defeating
   alias analysis, load/store forwarding, register promotion, and auto-vectorization.

3. **Copy recalculates offsets.** Copying an OffsetBox doesn't just copy the offset — it
   reconstructs the target pointer from the source, then recomputes a new offset relative to
   the destination's address. This prevents the compiler from treating copies as simple
   register moves.

### Standard conformance: separation of concerns

The C++ standard ([expr.reinterpret.cast]/5) guarantees:

> "A pointer converted to an integer of sufficient size and back to the same pointer type
> will have its original value; mappings between pointers and integers are **otherwise
> implementation-defined**."

The key insight is that this guarantee only covers **unmodified round-trips**: the integer
value passed to `reinterpret_cast<T*>()` must be exactly the value originally obtained from
`reinterpret_cast<intptr_t>(ptr)`. Integer arithmetic applied between the two casts places
the result in the "otherwise implementation-defined" category.

The `score::nothrow` and `score::memory::shared` designs differ fundamentally in how they handle
this constraint.

#### `score::nothrow::OffsetBox` — well-defined by construction

The `score::nothrow` design cleanly separates two concerns:

1. **OffsetBox: pointer identity preservation.** The OffsetBox only stores and reproduces
   pointers. Within a single process, the `this` address of the OffsetBox is unchanged
   between construction and conversion:

   ```cpp
   // Construction:
   offset_ = intptr_t(ptr) - intptr_t(this)

   // Conversion (this unchanged):
   intptr_t(this) + offset_
     = intptr_t(this) + intptr_t(ptr) - intptr_t(this)
     = intptr_t(ptr)   // algebraic identity
   ```

   The `reinterpret_cast<T*>()` always receives the **original, unmodified** integer value
   of the pointer. This is the guaranteed round-trip per [expr.reinterpret.cast]/5.

   **Copy and move semantics preserve this property.** The copy constructor and assignment
   operator always transfer state via the raw pointer in the current address space, never
   by copying the offset directly:

   ```cpp
   // Copy construction — converts source to T* first, then computes fresh offset:
   OffsetBox(const OffsetBox& other)
       : offset_{intptr_t(static_cast<T*>(other)) - intptr_t(this)} {}

   // Assignment — same pattern:
   OffsetBox& operator=(const OffsetBox& other) {
       offset_ = intptr_t(static_cast<T*>(other)) - intptr_t(this);
       return *this;
   }
   ```

   The source's conversion `static_cast<T*>(other)` is a guaranteed round-trip (the
   source's `this` is unchanged). The destination then stores a fresh offset relative to
   its own address. At no point is an offset value copied between two OffsetBox instances
   — the raw pointer is always the transfer medium, and each OffsetBox independently
   maintains a well-defined round-trip with its own `this`.

2. **Vector: array pointer arithmetic.** All element-level operations (`data[i]`, `data + n`,
   iterator arithmetic) use raw `T*` obtained from the allocator. The Vector knows it is
   operating within an allocated array, so this is well-defined pointer arithmetic per
   [expr.add].

These two concerns never cross: OffsetBox never does array arithmetic, and Vector never does
offset-based pointer reconstruction.

#### `score::memory::shared::OffsetPtr` — conflates both concerns

The existing design uses OffsetBox as a full **fancy pointer** (`allocator_traits::pointer`).
This means `std::vector` delegates *all* pointer operations to OffsetBox, including:

- `operator+`, `operator-` (element advancement within arrays)
- `operator[]` (array subscript)
- `operator*`, `operator->` (dereference)

Every one of these operations routes through the `uintptr_t` conversion machinery. When
`std::vector` does `ptr + n` to access element `n`, the OffsetBox computes:

```
CastIntegerToPointer(CastPointerToInteger(this) + stored_offset + n * sizeof(T))
```

The integer passed to `CastIntegerToPointer` was not obtained from `CastPointerToInteger` —
it is a manufactured value. This is "otherwise implementation-defined" per the standard, for
every element access.

#### Shared memory: out of scope for the standard

The C++ standard makes no assumptions about shared memory. When a different process maps a
shared memory region and accesses an OffsetBox stored there, the OffsetBox's `this` address
differs from the original process. From the reading process's perspective, the OffsetBox is
simply constructed (via the copy constructor or equivalent) with values that produce a valid
pointer *within that process's address space*. This is a platform concern, not a language
concern — both designs handle it equivalently.

### Potential improvements

- **Move `CastPointerToInteger` to the header** (or mark `inline`). This would allow the
  compiler to inline the entire pointer reconstruction path, likely reducing the overhead
  dramatically while preserving the same semantics.
- **Separate pointer storage from array arithmetic** as demonstrated by `score::nothrow`. Use
  OffsetBox exclusively for storing pointers in shared memory. Use raw `T*` for all
  element-level operations in container code. This achieves both standard conformance
  (well-defined round-trips only) and performance (~15% overhead vs ~180×).
- **Investigate `std::launder`** or compiler-specific provenance intrinsics as a
  standard-conforming alternative.
- **Benchmark with LTO enabled** to measure the overhead when inlining is possible across
  translation units.
