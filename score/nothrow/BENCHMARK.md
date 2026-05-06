# OffsetSlot vs OffsetPtr Benchmark

## Overview

Measures the operational overhead of `OffsetSlot<T>` pointer semantics (offset arithmetic,
bounds checking) for:

- nested `Vector<Vector<int>>` random access, compared to raw `T*` pointers in `std::vector`
- map workloads (`std::map`, `score::memory::shared::Map`, `score::nothrow::Map`) covering
  random-order build, randomized key lookup and begin→end iteration

The benchmark uses a 1000×1000 nested vector with 100,000 random `(i, j)` element accesses.
The random access pattern prevents the CPU from caching resolved OffsetSlot base addresses
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
  //score/nothrow:pointer_slot_benchmark
```

> **Note:** The `--per_file_copt` flag relaxes strict compiler warnings for external
> dependencies (Google Benchmark). This is needed because the project toolchain enables
> `-Werror` globally, which causes build failures in third-party code.

**Run:**

```bash
bazel-bin/score/nothrow/pointer_slot_benchmark
```

Or build and run in one step:

```bash
bazel run -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_slot_benchmark
```

**Run only vector + map family comparisons (excluding `*_Internal_*`):**

```bash
bazel run -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  //score/nothrow:pointer_slot_benchmark -- \
  --benchmark_filter='^BM_(StdVector_RandomAccess|MemorySharedVector_(NoBoundsCheck|BoundsChecked)_RandomAccess|RawSlotVector_RandomAccess|OffsetSlotVector_RandomAccess|BoostInterprocessVector_RandomAccess|StdMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|MemorySharedMap_(NoBoundsCheck|BoundsChecked)_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|RawSlotMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|OffsetSlotMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance))|BoostInterprocessMap_(RandomBuild|RandomAccess|Iterate|Internal_(InsertRebalance|Find|EraseRebalance)))$' \
  --benchmark_min_time=0.05s
```

### With `-O3`

```bash
bazel build -c opt \
  --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' \
  --per_file_copt='.*@-O3,-Wno-error=maybe-uninitialized' \
  //score/nothrow:pointer_slot_benchmark
```

## Benchmark Variants

Naming convention used in this benchmark:

- `Std*`: standard library baseline (`std::vector`, `std::map`)
- `RawSlot*`: `score::nothrow::*` with direct-pointer policy (`RawSlotPolicy`)
- `OffsetSlot*`: `score::nothrow::*` with relocatable offset-pointer policy
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
| `BM_RawSlotVector_RandomAccess` | `score::nothrow::Vector` with direct-pointer policy (`RawSlotPolicy`) |
| `BM_OffsetSlotVector_RandomAccess` | `score::nothrow::Vector` with relocatable offset-pointer policy |
| `BM_BoostInterprocessVector_RandomAccess` | (linux) `boost::interprocess::vector<vector<int>>` with `offset_ptr` over heap-backed segment manager |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | `score::memory::shared::Vector` `OffsetPtr` offset-arithmetic overhead only (global bounds check disabled) |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | `score::memory::shared::Vector` `OffsetPtr` offset arithmetic + bounds validation |
| `BM_StdMap_RandomBuild` | Baseline: `std::map<int,int>` random-order insertion |
| `BM_RawSlotMap_RandomBuild` | `score::nothrow::Map<int,int>` with direct-pointer policy |
| `BM_OffsetSlotMap_RandomBuild` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy |
| `BM_BoostInterprocessMap_RandomBuild` | (linux) `boost::interprocess::map<int,int>` random-order insertion |
| `BM_MemorySharedMap_NoBoundsCheck_RandomBuild` | `score::memory::shared::Map<int,int>` random-order insertion (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_RandomBuild` | `score::memory::shared::Map<int,int>` random-order insertion (global bounds check enabled) |
| `BM_StdMap_RandomAccess` | Baseline: `std::map<int,int>` randomized key lookup |
| `BM_RawSlotMap_RandomAccess` | `score::nothrow::Map<int,int>` with direct-pointer policy randomized key lookup |
| `BM_OffsetSlotMap_RandomAccess` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy randomized key lookup |
| `BM_BoostInterprocessMap_RandomAccess` | (linux) `boost::interprocess::map<int,int>` randomized key lookup |
| `BM_MemorySharedMap_NoBoundsCheck_RandomAccess` | `score::memory::shared::Map<int,int>` randomized key lookup (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_RandomAccess` | `score::memory::shared::Map<int,int>` randomized key lookup (global bounds check enabled) |
| `BM_StdMap_Iterate` | Baseline: `std::map<int,int>` begin→end iteration |
| `BM_RawSlotMap_Iterate` | `score::nothrow::Map<int,int>` with direct-pointer policy begin→end iteration |
| `BM_OffsetSlotMap_Iterate` | `score::nothrow::Map<int,int>` with relocatable offset-pointer policy begin→end iteration |
| `BM_BoostInterprocessMap_Iterate` | (linux) `boost::interprocess::map<int,int>` begin→end iteration |
| `BM_MemorySharedMap_NoBoundsCheck_Iterate` | `score::memory::shared::Map<int,int>` begin→end iteration (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Iterate` | `score::memory::shared::Map<int,int>` begin→end iteration (global bounds check enabled) |
| `BM_StdMap_Internal_InsertRebalance` | Focused std::map insert/rebalance path using ascending insert order |
| `BM_RawSlotMap_Internal_InsertRebalance` | Focused score::nothrow::Map insert/rebalance path using ascending insert order |
| `BM_OffsetSlotMap_Internal_InsertRebalance` | Focused score::nothrow::Map (OffsetSlot policy) insert/rebalance path |
| `BM_BoostInterprocessMap_Internal_InsertRebalance` | (linux) focused boost::interprocess::map insert/rebalance path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance` | Focused score::memory::shared::Map insert/rebalance path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance` | Focused score::memory::shared::Map insert/rebalance path (global bounds check enabled) |
| `BM_StdMap_Internal_Find` | Focused std::map find hot path over randomized successful lookups |
| `BM_RawSlotMap_Internal_Find` | Focused score::nothrow::Map find hot path over randomized successful lookups |
| `BM_OffsetSlotMap_Internal_Find` | Focused score::nothrow::Map (OffsetSlot policy) find hot path |
| `BM_BoostInterprocessMap_Internal_Find` | (linux) focused boost::interprocess::map find hot path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_Find` | Focused score::memory::shared::Map find path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_Find` | Focused score::memory::shared::Map find path (global bounds check enabled) |
| `BM_StdMap_Internal_EraseRebalance` | Focused std::map erase/rebalance path using randomized erase order |
| `BM_RawSlotMap_Internal_EraseRebalance` | Focused score::nothrow::Map erase/rebalance path using randomized erase order |
| `BM_OffsetSlotMap_Internal_EraseRebalance` | Focused score::nothrow::Map (OffsetSlot policy) erase/rebalance path |
| `BM_BoostInterprocessMap_Internal_EraseRebalance` | (linux) focused boost::interprocess::map erase/rebalance path |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance` | Focused score::memory::shared::Map erase/rebalance path (global bounds check disabled) |
| `BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance` | Focused score::memory::shared::Map erase/rebalance path (global bounds check enabled) |

## Results

Measured on host `marlin` with benchmark context:

- 16 logical CPUs @ ~5135 MHz
- L3 cache: 16 MiB
- `cpu_scaling_enabled: true`
- build: `bazel run -c opt --per_file_copt='external/.*@-Wno-undef,-Wno-suggest-attribute=format,-Wno-error' //score/nothrow:pointer_slot_benchmark`
- runtime flags: `--benchmark_min_time=0.3s --benchmark_repetitions=3 --benchmark_report_aggregates_only=true`

Element type: `int` (4 bytes) in `Vector<Vector<int>>` with 1000×1000 elements and 100,000 random accesses.

### Full benchmark suite (mean CPU time, ns)

| Benchmark | Mean CPU (ns) |
|---|---:|
| `BM_StdVector_RandomAccess` | 111,631 |
| `BM_RawSlotVector_RandomAccess` | 112,542 |
| `BM_OffsetSlotVector_RandomAccess` | 125,386 |
| `BM_BoostInterprocessVector_RandomAccess` | 175,150 |
| `BM_MemorySharedVector_NoBoundsCheck_RandomAccess` | 4,132,046 |
| `BM_MemorySharedVector_BoundsChecked_RandomAccess` | 19,381,986 |
| `BM_StdMap_RandomBuild` | 903,817 |
| `BM_RawSlotMap_RandomBuild` | 922,110 |
| `BM_OffsetSlotMap_RandomBuild` | 1,160,195 |
| `BM_BoostInterprocessMap_RandomBuild` | 8,313,924 |
| `BM_MemorySharedMap_NoBoundsCheck_RandomBuild` | 17,858,405 |
| `BM_MemorySharedMap_BoundsChecked_RandomBuild` | 77,908,034 |
| `BM_StdMap_RandomAccess` | 3,316,251 |
| `BM_RawSlotMap_RandomAccess` | 3,187,163 |
| `BM_OffsetSlotMap_RandomAccess` | 6,708,715 |
| `BM_BoostInterprocessMap_RandomAccess` | 10,673,484 |
| `BM_MemorySharedMap_NoBoundsCheck_RandomAccess` | 98,084,979 |
| `BM_MemorySharedMap_BoundsChecked_RandomAccess` | 452,350,010 |
| `BM_StdMap_Iterate` | 48,513 |
| `BM_RawSlotMap_Iterate` | 47,639 |
| `BM_OffsetSlotMap_Iterate` | 71,813 |
| `BM_BoostInterprocessMap_Iterate` | 113,081 |
| `BM_MemorySharedMap_NoBoundsCheck_Iterate` | 1,875,049 |
| `BM_MemorySharedMap_BoundsChecked_Iterate` | 8,247,345 |
| `BM_StdMap_Internal_InsertRebalance` | 221,707 |
| `BM_RawSlotMap_Internal_InsertRebalance` | 134,369 |
| `BM_OffsetSlotMap_Internal_InsertRebalance` | 157,464 |
| `BM_BoostInterprocessMap_Internal_InsertRebalance` | 4,852,561 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance` | 14,645,197 |
| `BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance` | 65,883,342 |
| `BM_StdMap_Internal_Find` | 2,886,804 |
| `BM_RawSlotMap_Internal_Find` | 2,752,042 |
| `BM_OffsetSlotMap_Internal_Find` | 5,289,642 |
| `BM_BoostInterprocessMap_Internal_Find` | 9,178,041 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_Find` | 90,707,088 |
| `BM_MemorySharedMap_BoundsChecked_Internal_Find` | 422,901,546 |
| `BM_StdMap_Internal_EraseRebalance` | 556,448 |
| `BM_RawSlotMap_Internal_EraseRebalance` | 607,222 |
| `BM_OffsetSlotMap_Internal_EraseRebalance` | 771,740 |
| `BM_BoostInterprocessMap_Internal_EraseRebalance` | 5,986,049 |
| `BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance` | 15,106,480 |
| `BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance` | 65,559,348 |

### Ratio summary (vs std baseline in each family)

| Comparison | Ratio |
|---|---:|
| `RawSlotVector_RandomAccess / StdVector_RandomAccess` | 1.01× |
| `OffsetSlotVector_RandomAccess / StdVector_RandomAccess` | 1.12× |
| `BoostInterprocessVector_RandomAccess / StdVector_RandomAccess` | 1.57× |
| `MemorySharedVector_NoBoundsCheck / StdVector_RandomAccess` | 37.02× |
| `MemorySharedVector_BoundsChecked / StdVector_RandomAccess` | 173.63× |
| `RawSlotMap_RandomBuild / StdMap_RandomBuild` | 1.02× |
| `OffsetSlotMap_RandomBuild / StdMap_RandomBuild` | 1.28× |
| `BoostInterprocessMap_RandomBuild / StdMap_RandomBuild` | 9.20× |
| `MemorySharedMap_NoBoundsCheck_RandomBuild / StdMap_RandomBuild` | 19.76× |
| `MemorySharedMap_BoundsChecked_RandomBuild / StdMap_RandomBuild` | 86.20× |
| `RawSlotMap_RandomAccess / StdMap_RandomAccess` | 0.96× |
| `OffsetSlotMap_RandomAccess / StdMap_RandomAccess` | 2.02× |
| `BoostInterprocessMap_RandomAccess / StdMap_RandomAccess` | 3.22× |
| `MemorySharedMap_NoBoundsCheck_RandomAccess / StdMap_RandomAccess` | 29.58× |
| `MemorySharedMap_BoundsChecked_RandomAccess / StdMap_RandomAccess` | 136.40× |
| `RawSlotMap_Iterate / StdMap_Iterate` | 0.98× |
| `OffsetSlotMap_Iterate / StdMap_Iterate` | 1.48× |
| `BoostInterprocessMap_Iterate / StdMap_Iterate` | 2.33× |
| `MemorySharedMap_NoBoundsCheck_Iterate / StdMap_Iterate` | 38.65× |
| `MemorySharedMap_BoundsChecked_Iterate / StdMap_Iterate` | 170.00× |
| `RawSlotMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 0.61× |
| `OffsetSlotMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 0.71× |
| `BoostInterprocessMap_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 21.89× |
| `MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 66.06× |
| `MemorySharedMap_BoundsChecked_Internal_InsertRebalance / StdMap_Internal_InsertRebalance` | 297.16× |
| `RawSlotMap_Internal_Find / StdMap_Internal_Find` | 0.95× |
| `OffsetSlotMap_Internal_Find / StdMap_Internal_Find` | 1.83× |
| `BoostInterprocessMap_Internal_Find / StdMap_Internal_Find` | 3.18× |
| `MemorySharedMap_NoBoundsCheck_Internal_Find / StdMap_Internal_Find` | 31.42× |
| `MemorySharedMap_BoundsChecked_Internal_Find / StdMap_Internal_Find` | 146.49× |
| `RawSlotMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 1.09× |
| `OffsetSlotMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 1.39× |
| `BoostInterprocessMap_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 10.76× |
| `MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 27.15× |
| `MemorySharedMap_BoundsChecked_Internal_EraseRebalance / StdMap_Internal_EraseRebalance` | 117.82× |

### Current takeaways

- Benchmarks are listed in regular scenario order (VectorRandomAccess, MapRandomBuild, MapRandomAccess, MapIterate, then map internal microbenchmarks), and within every scenario use a consistent container order: Std, RawSlot, OffsetSlot, BoostInterprocess (linux), MemoryShared NoBoundsCheck, MemoryShared BoundsChecked.
- For fair container-to-container comparison, use the `NoBoundsCheck` rows: `score::memory::shared` is still ~20–66× slower than `std` across these scenarios (including internal microbenchmarks).
- `score::memory::shared::Map` now includes both no-bounds-check and bounds-checked variants across map scenarios and internal microbenchmarks using the same bounded resource configuration.
- `BoostInterprocess` is now directly included in all benchmark scenarios and ratio rows. Resource/heap setup is excluded from timing in all `MemoryShared` and `BoostInterprocess` benchmarks for fair comparison with `Std` and `nothrow`.
- `RawSlot` is near/at parity for map lookup and iteration, and faster in focused insert/rebalance and find microbenchmarks.
- `RawSlotMap` random build remains slightly slower than `std::map`.
- `RawSlotMap` erase/rebalance remains slower than `std::map` and is the primary remaining gap.
- `score::memory::shared::*` remains far slower in this benchmark, especially for random map access and vector random access.

### Cross-validation with independent implementation

An independent OffsetSlot Vector implementation
([TrivialityInversion](https://github.com/jmd-emermern/TrivialityInversion), same
`intptr_t`-based inline approach) shows comparable results on the same hardware using
CMake Release builds (`-O3`):

| Element type | `std::vector` | OffsetSlot Vector | Overhead |
|---|---|---|---|
| `uint8_t` | 1.10 ms | 1.24 ms | ~12% |
| `uint32_t` | 1.38 ms | 1.49 ms | ~8% |
| `uint64_t` | 3.13 ms | 3.35 ms | ~7% |

The overhead varies by element size (smaller elements → higher relative overhead, likely
due to the constant per-access OffsetSlot conversion cost being larger relative to the
cheaper cache-line loads for small elements).

### Summary

| Configuration | Inline OffsetSlot overhead | Cross-TU OffsetPtr overhead |
|---|---|---|
| Bazel `-O2` (default) | ~1–14% | ~20–66× (NoBoundsCheck), up to ~297× (BoundsChecked) |
| CMake `-O3` (cross-validation) | ~7–12% | n/a |

**Key findings:**

- **The existing `score::memory::shared::OffsetPtr` implementation is ~20–66× in NoBoundsCheck mode and up to ~297× in BoundsChecked mode** depending on container path,
  dominated by cross-translation-unit call barriers in `pointer_arithmetic_util.cpp` and
  the `uintptr_t` round-trip pattern that destroys pointer provenance for the optimizer.
- **A header-inline OffsetSlot reduces overhead to ~1–14%** vs raw pointers. The `intptr_t`-based
  offset computation is fully visible to the compiler, allowing it to optimize through the
  pointer reconstruction. This confirms that the overhead is not inherent to relative pointers
  but caused by the implementation strategy.
- **Bounds checking can add substantial overhead** on top of existing offset arithmetic costs, so NoBoundsCheck is the fair baseline for cross-category comparisons here.
  In the inline implementation, this cost would be proportionally smaller.
- **This is a worst-case benchmark** — random access with double indirection through nested
  containers. Sequential access patterns would show lower overhead due to CPU prefetching.

## Analysis: Root Cause of score::memory::shared::OffsetPtr Overhead

The large overhead in the current implementation (~20–66× in no-bounds mode and up to ~297× in bounds-checked mode) is not inherent to the concept of relative pointers. It is caused by the
implementation strategy chosen to avoid using built-in pointer arithmetic between unrelated
objects, and to quarantine the remaining pointer/integer mapping behind optimization barriers.

### The problem

`OffsetPtr<T>` stores a byte offset from its own address to the target object. To reconstruct
the target pointer, it must compute `this_address + offset`. However, direct pointer arithmetic
(`ptr + n`) is only well-defined within an array or one past its end (C++ [expr.add]). The
`OffsetPtr` object and its target are in different allocations, so direct pointer arithmetic would be UB.

### The chosen solution: cast through `uintptr_t`

`score::memory::shared::OffsetPtr` reconstructs pointers by converting addresses to integers,
performing integer arithmetic there, and then casting back:

```cpp
uintptr_t CastPointerToInteger(const void* ptr) {
    return reinterpret_cast<std::uintptr_t>(ptr);
}
// ... integer arithmetic ...
T* CastIntegerToPointer(uintptr_t integer) {
    return reinterpret_cast<T*>(integer);
}
```

The call chain for `operator*()`, `operator->()`, and `operator[]()` is:

```
GetPointerWithBoundsCheck → CalculatePointerFromOffset → AddOffsetToPointer
  → CastPointerToInteger → integer add → CastIntegerToPointer
```

In this implementation, `CastPointerToInteger` lives in `pointer_arithmetic_util.cpp`, so without
LTO the compiler cannot inline the whole sequence. This acts as an **optimization barrier** around
the pointer/integer/pointer conversion.

That barrier does **not** make the code more standard-conforming. Its purpose is to keep the
compiler from seeing through the entire reconstruction pipeline and reasoning about the resulting
pointers as if they had ordinary source-level provenance.

### Why this is expensive

1. **Cross-translation-unit call barrier.** `CastPointerToInteger` is defined in
   `pointer_arithmetic_util.cpp`, not in the header. Without LTO, the compiler cannot inline
   it, so every dereference involves an actual function call. This alone likely accounts for
   the majority of the overhead.

2. **The optimizer cannot treat reconstructed pointers like ordinary `T*`.** The barrier hides
   the full cast/add/cast sequence from the optimizer. This prevents many aggressive
   transformations across the boundary, including scalar replacement, load/store forwarding,
   register promotion, and some alias-based simplifications. Boost.Interprocess uses a different
   mechanism (`volatile` helper functions) for the same reason: not to change the language rules,
   but to stop the compiler from seeing too much of the non-ordinary reconstruction.

3. **Copy recalculates offsets.** Copying an OffsetPtr doesn't just copy the offset — it
   reconstructs the target pointer from the source, then recomputes a new offset relative to
   the destination's address. This prevents the compiler from treating copies as simple
   register moves.

### Standard conformance: separation of concerns

The C++ standard ([expr.reinterpret.cast]/5) guarantees:

> "A pointer converted to an integer of sufficient size and back to the same pointer type
> will have its original value; mappings between pointers and integers are **otherwise
> implementation-defined**."

The key distinction is whether the integer passed to `reinterpret_cast<T*>()` is the same value
originally obtained from the pointer. If so, the result is the guaranteed identity round-trip.
If not, the mapping falls into the standard's "otherwise implementation-defined" bucket.

The `score::nothrow` and `score::memory::shared` designs differ fundamentally in how they handle
this constraint.

#### `score::nothrow::OffsetSlot` — identity round-trip only

The `score::nothrow` design cleanly separates two concerns:

1. **OffsetSlot: pointer identity preservation.** The OffsetSlot only stores and reproduces
   pointers. Within a single process, the `this` address of the OffsetSlot is unchanged
   between construction and conversion:

   ```cpp
   // Construction:
   offset_ = intptr_t(ptr) - intptr_t(this)

   // Conversion (this unchanged):
   intptr_t(this) + offset_
     = intptr_t(this) + intptr_t(ptr) - intptr_t(this)
     = intptr_t(ptr)   // algebraic identity
   ```

   The `reinterpret_cast<T*>()` receives the **original pointer value** again. This is the
   guaranteed round-trip per [expr.reinterpret.cast]/5.

   **Copy and move semantics preserve this property.** The copy constructor and assignment
   operator always transfer state via the raw pointer in the current address space, never
   by copying the offset directly:

   ```cpp
   // Copy construction — converts source to T* first, then computes fresh offset:
   OffsetSlot(const OffsetSlot& other)
       : offset_{intptr_t(static_cast<T*>(other)) - intptr_t(this)} {}

   // Assignment — same pattern:
   OffsetSlot& operator=(const OffsetSlot& other) {
       offset_ = intptr_t(static_cast<T*>(other)) - intptr_t(this);
       return *this;
   }
   ```

   The source's conversion `static_cast<T*>(other)` follows the same identity-round-trip
   argument. The destination then stores a fresh offset relative to its own address. At no
   point is an offset value copied between two OffsetSlot instances — the raw pointer is always
   the transfer medium, and each OffsetSlot independently reconstructs its pointee by returning
   to the original pointer value.

2. **Vector: array pointer arithmetic.** All element-level operations (`data[i]`, `data + n`,
   iterator arithmetic) use raw `T*` obtained from the allocator. The Vector knows it is
   operating within an allocated array, so this is well-defined pointer arithmetic per
   [expr.add].

These two concerns never cross: OffsetSlot never does array arithmetic, and Vector never does
offset-based pointer reconstruction. Because `OffsetSlot` only needs the standard's
pointer-to-integer-to-same-pointer guarantee, it does not need an optimization barrier for
semantic correctness.

#### `score::memory::shared::OffsetPtr` — conflates storage and traversal

The existing design uses OffsetPtr as a full **fancy pointer** (`allocator_traits::pointer`).
This means `std::vector` delegates *all* pointer operations to OffsetPtr, including:

- `operator+`, `operator-` (element advancement within arrays)
- `operator[]` (array subscript)
- `operator*`, `operator->` (dereference)

Some of these operations reconstruct the current pointee, but the design must also synthesize
**different element pointers** during traversal. When `std::vector` does `ptr + n` to access
element `n`, `OffsetPtr` computes:

```
CastIntegerToPointer(CastPointerToInteger(this) + stored_offset + n * sizeof(T))
```

For `n != 0`, the integer passed to `CastIntegerToPointer` is not the original pointer value —
it is a derived value for a different element. That is exactly the "otherwise
implementation-defined" case from [expr.reinterpret.cast]/5.

This is why the design needs optimization barriers and why removing them is risky. If the
compiler can see directly through the full cast/add/cast sequence on these traversal paths, it
is free to optimize using its own aliasing and provenance model rather than the flat-address
machine model that the implementation intends. The out-of-line helper functions in
`score::memory::shared` and the `volatile` helper functions in Boost.Interprocess are both ways
to quarantine that reconstruction from overly aggressive optimization. They do not make the
mapping more defined; they just make miscompilation less likely on mainstream compilers.

#### Shared memory: out of scope for the standard

The C++ standard makes no assumptions about shared memory. When a different process maps a
shared memory region and accesses an OffsetSlot stored there, the OffsetSlot's `this` address
differs from the original process. From the reading process's perspective, the OffsetSlot is
simply constructed (via the copy constructor or equivalent) with values that produce a valid
pointer *within that process's address space*. This is a platform concern, not a language
concern — both designs handle it equivalently.
