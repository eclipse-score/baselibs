# OffsetBox vs OffsetPtr Benchmark

## Overview

Measures the operational overhead of `OffsetBox<T>` pointer semantics (offset arithmetic,
bounds checking) for:

- nested `Vector<Vector<int>>` random access, compared to raw `T*` pointers in `std::vector`
- map workloads (`std::map`, `score::memory::shared::Map`, `score::nothrow::Map`) covering
  random-order build, randomized key lookup and begin→end iteration

The benchmark uses a 1000×1000 nested vector with 100,000 random `(i, j)` element accesses.
The random access pattern prevents the CPU from caching resolved OffsetBox base addresses
in registers, exercising the full offset-arithmetic path on every dereference.

Map workload configuration:

- Primary map scenarios (`RandomBuild`, `RandomAccess`, `Iterate`) use 10,000 key/value pairs.
- `RandomAccess` performs 100,000 successful lookups sampled from inserted keys.
- Focused internal map scenarios (`*Internal_*`) use 4,096 key/value pairs.
- `Internal_Find` performs 100,000 successful lookups.

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

**Run only vector + map family comparisons (excluding `*_Internal_*`):**

```bash
bazel run -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_box_benchmark -- \
  --benchmark_filter='^BM_(StdVector_RandomAccess|MemorySharedVector_(NoBoundsCheck|BoundsChecked)_RandomAccess|RawBoxVector_RandomAccess|OffsetBoxVector_RandomAccess|BoostInterprocessVector_RandomAccess|StdMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|MemorySharedMap_(NoBoundsCheck|BoundsChecked)_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|RawBoxMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|OffsetBoxMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|BoostInterprocessMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance)))$' \
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
- `RawBox*`: `score::nothrow::*` with direct-pointer policy (`RawBoxPolicy`)
- `OffsetBox*`: `score::nothrow::*` with relocatable offset-pointer policy
- `BoostInterprocess*` (linux): `boost::interprocess` containers using `offset_ptr` allocators over `managed_heap_memory`
- `MemoryShared*`: `score::memory::shared::*` implementation family

**Allocator note:** `std::*` and `score::nothrow::*` containers both use per-node
`new`/`delete` allocation, so their comparison is allocation-fair. `score::memory::shared::*`
uses a bump allocator (`MyBoundedMemoryResource`) that never reclaims freed memory, and
`boost::interprocess::*` uses a managed heap segment. These allocator differences are inherent
to each implementation's design and affect allocation-heavy benchmarks (RandomBuild,
InsertRebalance, EraseRebalance) but not lookup or iteration benchmarks where data is
pre-built.

| Benchmark | What it measures |
|---|---|
| `BM_StdVector_RandomAccess` | Baseline: `std::vector<std::vector<int>>` with raw `T*` pointers |
| `BM_RawBoxVector_RandomAccess` | `score::nothrow::Vector` with direct-pointer policy (`RawBoxPolicy`) |
| `BM_OffsetBoxVector_RandomAccess` | `score::nothrow::Vector` with relocatable offset-pointer policy |
| `BM_BoostInterprocessVector_RandomAccess` | (linux) `boost::interprocess::vector<vector<int>>` with `offset_ptr` over heap-backed segment manager |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | `score::memory::shared::Vector` OffsetBox offset-arithmetic overhead only (global bounds check disabled) |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | `score::memory::shared::Vector` OffsetBox offset arithmetic + bounds validation |
| `BM_StdMap_RandomBuild` | Baseline: `std::map<int,int>` random-order insertion |
| `BM_RawBoxMap_RandomBuild` | `score::nothrow::Map<int,int>` with direct-pointer policy |
| `BM_OffsetBoxMap_RandomBuild` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy |
| `BM_BoostInterprocessMap_RandomBuild` | (linux) `boost::interprocess::map<int,int>` random-order insertion |
| `BM_MemorySharedMap_NoBoundsCheck_RandomBuild` | `score::memory::shared::Map<int,int>` random-order insertion (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_RandomBuild` | `score::memory::shared::Map<int,int>` random-order insertion (global bounds check enabled) |
| `BM_StdMap_RandomAccess` | Baseline: `std::map<int,int>` randomized key lookup |
| `BM_RawBoxMap_RandomAccess` | `score::nothrow::Map<int,int>` with direct-pointer policy randomized key lookup |
| `BM_OffsetBoxMap_RandomAccess` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy randomized key lookup |
| `BM_BoostInterprocessMap_RandomAccess` | (linux) `boost::interprocess::map<int,int>` randomized key lookup |
| `BM_MemorySharedMap_NoBoundsCheck_RandomAccess` | `score::memory::shared::Map<int,int>` randomized key lookup (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_RandomAccess` | `score::memory::shared::Map<int,int>` randomized key lookup (global bounds check enabled) |
| `BM_StdMap_Iterate` | Baseline: `std::map<int,int>` begin→end iteration |
| `BM_RawBoxMap_Iterate` | `score::nothrow::Map<int,int>` with direct-pointer policy begin→end iteration |
| `BM_OffsetBoxMap_Iterate` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy begin→end iteration |
| `BM_BoostInterprocessMap_Iterate` | (linux) `boost::interprocess::map<int,int>` begin→end iteration |
| `BM_MemorySharedMap_NoBoundsCheck_Iterate` | `score::memory::shared::Map<int,int>` begin→end iteration (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Iterate` | `score::memory::shared::Map<int,int>` begin→end iteration (global bounds check enabled) |
| `BM_StdMap_Internal_InsertRebalance` | Focused std::map insert/rebalance path using ascending insert order |
| `BM_RawBoxMap_Internal_InsertRebalance` | Focused score::nothrow::Map insert/rebalance path using ascending insert order |
| `BM_OffsetBoxMap_Internal_InsertRebalance` | Focused score::nothrow::Map (OffsetBox policy) insert/rebalance path |
| `BM_BoostInterprocessMap_Internal_InsertRebalance` | (linux) focused boost::interprocess::map insert/rebalance path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance` | Focused score::memory::shared::Map insert/rebalance path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance` | Focused score::memory::shared::Map insert/rebalance path (global bounds check enabled) |
| `BM_StdMap_Internal_Find` | Focused std::map find hot path over randomized successful lookups |
| `BM_RawBoxMap_Internal_Find` | Focused score::nothrow::Map find hot path over randomized successful lookups |
| `BM_OffsetBoxMap_Internal_Find` | Focused score::nothrow::Map (OffsetBox policy) find hot path |
| `BM_BoostInterprocessMap_Internal_Find` | (linux) focused boost::interprocess::map find hot path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_Find` | Focused score::memory::shared::Map find path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_Find` | Focused score::memory::shared::Map find path (global bounds check enabled) |
| `BM_StdMap_Internal_EraseRebalance` | Focused std::map erase/rebalance path using randomized erase order |
| `BM_RawBoxMap_Internal_EraseRebalance` | Focused score::nothrow::Map erase/rebalance path using randomized erase order |
| `BM_OffsetBoxMap_Internal_EraseRebalance` | Focused score::nothrow::Map (OffsetBox policy) erase/rebalance path |
| `BM_BoostInterprocessMap_Internal_EraseRebalance` | (linux) focused boost::interprocess::map erase/rebalance path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance` | Focused score::memory::shared::Map erase/rebalance path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance` | Focused score::memory::shared::Map erase/rebalance path (global bounds check enabled) |

## Results

Measured on host `marlin` with benchmark context:

- 16 logical CPUs @ ~5135 MHz
- L3 cache: 16 MiB
- `cpu_scaling_enabled: true`
- build: `bazel run -c opt --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' //score/nothrow:pointer_box_benchmark`
- runtime flags: `--benchmark_min_time=0.3s --benchmark_repetitions=3 --benchmark_report_aggregates_only=true`

Element type: `int` (4 bytes) in `Vector<Vector<int>>` with 1000×1000 elements and 100,000 random accesses.

### Full benchmark suite (mean CPU time, ns)

| Benchmark | Mean CPU (ns) |
|---|---:|
| `BM_StdVector_RandomAccess` | 131,711 |
| `BM_RawBoxVector_RandomAccess` | 132,329 |
| `BM_OffsetBoxVector_RandomAccess` | 150,584 |
| `BM_BoostInterprocessVector_RandomAccess` | 203,596 |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | 5,513,105 |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | 23,635,204 |
| `BM_StdMap_RandomBuild` | 838,515 |
| `BM_RawBoxMap_RandomBuild` | 895,846 |
| `BM_OffsetBoxMap_RandomBuild` | 1,200,038 |
| `BM_BoostInterprocessMap_RandomBuild` | 9,334,875 |
| `BM_MemorySharedMap_NoBoundsCheck_RandomBuild` | 18,847,630 |
| `BM_MemorySharedMap_BoundsChecked_RandomBuild` | 80,401,934 |
| `BM_StdMap_RandomAccess` | 3,446,061 |
| `BM_RawBoxMap_RandomAccess` | 3,368,458 |
| `BM_OffsetBoxMap_RandomAccess` | 7,293,329 |
| `BM_BoostInterprocessMap_RandomAccess` | 10,963,653 |
| `BM_MemorySharedMap_NoBoundsCheck_RandomAccess` | 101,729,710 |
| `BM_MemorySharedMap_BoundsChecked_RandomAccess` | 467,278,508 |
| `BM_StdMap_Iterate` | 48,081 |
| `BM_RawBoxMap_Iterate` | 47,621 |
| `BM_OffsetBoxMap_Iterate` | 75,444 |
| `BM_BoostInterprocessMap_Iterate` | 119,128 |
| `BM_MemorySharedMap_NoBoundsCheck_Iterate` | 1,955,376 |
| `BM_MemorySharedMap_BoundsChecked_Iterate` | 8,576,551 |
| `BM_StdMap_Internal_InsertRebalance` | 180,522 |
| `BM_RawBoxMap_Internal_InsertRebalance` | 124,064 |
| `BM_OffsetBoxMap_Internal_InsertRebalance` | 138,851 |
| `BM_BoostInterprocessMap_Internal_InsertRebalance` | 5,829,919 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance` | 15,055,574 |
| `BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance` | 67,239,602 |
| `BM_StdMap_Internal_Find` | 2,952,186 |
| `BM_RawBoxMap_Internal_Find` | 2,783,739 |
| `BM_OffsetBoxMap_Internal_Find` | 5,395,071 |
| `BM_BoostInterprocessMap_Internal_Find` | 9,154,297 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_Find` | 92,126,468 |
| `BM_MemorySharedMap_BoundsChecked_Internal_Find` | 428,074,707 |
| `BM_StdMap_Internal_EraseRebalance` | 505,060 |
| `BM_RawBoxMap_Internal_EraseRebalance` | 582,973 |
| `BM_OffsetBoxMap_Internal_EraseRebalance` | 747,941 |
| `BM_BoostInterprocessMap_Internal_EraseRebalance` | 6,831,710 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance` | 15,482,016 |
| `BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance` | 66,203,230 |

### Ratio summary (vs std baseline in each family)

| Comparison | Ratio |
|---|---:|
| `RawBoxVector_RandomAccess / StdVector_RandomAccess` | 1.00× |
| `OffsetBoxVector_RandomAccess / StdVector_RandomAccess` | 1.14× |
| `BoostInterprocessVector_RandomAccess / StdVector_RandomAccess` | 1.55× |
| `MemorySharedVector_NoBoundsCheck / StdVector_RandomAccess` | 41.86× |
| `MemorySharedVector_BoundsChecked / StdVector_RandomAccess` | 179.45× |
| `RawBoxMap_RandomBuild / StdMap_RandomBuild` | 1.07× |
| `OffsetBoxMap_RandomBuild / StdMap_RandomBuild` | 1.43× |
| `BoostInterprocessMap_RandomBuild / StdMap_RandomBuild` | 11.13× |
| `MemorySharedMap_NoBoundsCheck_RandomBuild / StdMap_RandomBuild` | 22.48× |
| `MemorySharedMap_BoundsChecked_RandomBuild / StdMap_RandomBuild` | 95.89× |
| `RawBoxMap_RandomAccess / StdMap_RandomAccess` | 0.98× |
| `OffsetBoxMap_RandomAccess / StdMap_RandomAccess` | 2.12× |
| `BoostInterprocessMap_RandomAccess / StdMap_RandomAccess` | 3.18× |
| `MemorySharedMap_NoBoundsCheck_RandomAccess / StdMap_RandomAccess` | 29.52× |
| `MemorySharedMap_BoundsChecked_RandomAccess / StdMap_RandomAccess` | 135.60× |
| `RawBoxMap_Iterate / StdMap_Iterate` | 0.99× |
| `OffsetBoxMap_Iterate / StdMap_Iterate` | 1.57× |
| `BoostInterprocessMap_Iterate / StdMap_Iterate` | 2.48× |
| `MemorySharedMap_NoBoundsCheck_Iterate / StdMap_Iterate` | 40.67× |
| `MemorySharedMap_BoundsChecked_Iterate / StdMap_Iterate` | 178.38× |
| `RawBoxMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 0.69× |
| `OffsetBoxMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 0.77× |
| `BoostInterprocessMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 32.29× |
| `MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 83.40× |
| `MemorySharedMap_BoundsChecked_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 372.47× |
| `RawBoxMap_Internal_Find / StdMap_Internal_Find` | 0.94× |
| `OffsetBoxMap_Internal_Find / StdMap_Internal_Find` | 1.83× |
| `BoostInterprocessMap_Internal_Find / StdMap_Internal_Find` | 3.10× |
| `MemorySharedMap_NoBoundsCheck_Internal_Find / StdMap_Internal_Find` | 31.21× |
| `MemorySharedMap_BoundsChecked_Internal_Find / StdMap_Internal_Find` | 145.00× |
| `RawBoxMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 1.15× |
| `OffsetBoxMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 1.48× |
| `BoostInterprocessMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 13.53× |
| `MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 30.65× |
| `MemorySharedMap_BoundsChecked_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 131.08× |

### Current takeaways

- Benchmarks are listed in regular scenario order (VectorRandomAccess, MapRandomBuild, MapRandomAccess, MapIterate, then map internal microbenchmarks), and within every scenario use a consistent container order: Std, RawBox, OffsetBox, BoostInterprocess (linux), MemoryShared NoBoundsCheck, MemoryShared BoundsChecked.
- For fair container-to-container comparison, use the `NoBoundsCheck` rows: `score::memory::shared` is still ~22–83× slower than `std` across these scenarios (including internal microbenchmarks).
- `score::memory::shared::Map` now includes both no-bounds-check and bounds-checked variants across map scenarios and internal microbenchmarks using the same bounded resource configuration.
- `BoostInterprocess` is now directly included in all benchmark scenarios and ratio rows. Resource/heap setup is excluded from timing in all `MemoryShared` and `BoostInterprocess` benchmarks for fair comparison with `Std` and `nothrow`.
- `RawBox` is near/at parity for map lookup and iteration, and faster in focused insert/rebalance and find microbenchmarks.
- `RawBoxMap` random build remains slightly slower than `std::map`.
- `RawBoxMap` erase/rebalance remains slower than `std::map` and is the primary remaining gap.
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
| Bazel `-O2` (default) | ~1–14% | ~22–83× (NoBoundsCheck), up to ~372× (BoundsChecked) |
| CMake `-O3` (cross-validation) | ~7–12% | n/a |

**Key findings:**

- **The existing `score::memory::shared` OffsetBox is ~22–83× in NoBoundsCheck mode and up to ~372× in BoundsChecked mode** depending on container path,
  dominated by cross-translation-unit call barriers in `pointer_arithmetic_util.cpp` and
  the `uintptr_t` round-trip pattern that destroys pointer provenance for the optimizer.
- **A header-inline OffsetBox reduces overhead to ~1–14%** vs raw pointers. The `intptr_t`-based
  offset computation is fully visible to the compiler, allowing it to optimize through the
  pointer reconstruction. This confirms that the overhead is not inherent to relative pointers
  but caused by the implementation strategy.
- **Bounds checking can add substantial overhead** on top of existing offset arithmetic costs, so NoBoundsCheck is the fair baseline for cross-category comparisons here.
  In the inline implementation, this cost would be proportionally smaller.
- **This is a worst-case benchmark** — random access with double indirection through nested
  containers. Sequential access patterns would show lower overhead due to CPU prefetching.

## Analysis: Root Cause of score::memory::shared::OffsetPtr Overhead

The large overhead in the current implementation (~22–83× in no-bounds mode and up to ~372× in bounds-checked mode) is not inherent to the concept of relative pointers. It is caused by the
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

3. **Copy recalculates offsets.** Copying an OffsetPtr doesn't just copy the offset — it
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

The existing design uses OffsetPtr as a full **fancy pointer** (`allocator_traits::pointer`).
This means `std::vector` delegates *all* pointer operations to OffsetPtr, including:

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
  (well-defined round-trips only) and performance (~1–14% overhead vs ~22–83× in legacy/shared no-bounds paths).
- **Investigate `std::launder`** or compiler-specific provenance intrinsics as a
  standard-conforming alternative.
- **Benchmark with LTO enabled** to measure the overhead when inlining is possible across
  translation units.
