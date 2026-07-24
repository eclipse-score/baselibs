# memres — PMR Memory Resource Bridge

`memres` provides Rust code with owned, RAII handles to
[`score::cpp::pmr::memory_resource`](https://isocpp.org/files/papers/P0339R6.pdf)
objects from the `score::cpp` (futurecpp) C++ library.  It targets use-cases
where allocation behaviour must be tightly controlled — for example, preventing
any runtime heap use after initialisation.

---

## Purpose

`score::cpp::pmr` (Polymorphic Memory Resource) is the C++ standard-library
mechanism for decoupling containers from the allocator strategy used for their
storage.  `memres` makes two of those strategies available to Rust callers:

| Strategy | C++ type | Rust variant |
|---|---|---|
| Monotonic buffer | `score::cpp::pmr::monotonic_buffer_resource` | `ResourceKind::Monotonic { size }` |
| Single-threaded pool | `score::cpp::pmr::unsynchronized_pool_resource` | `ResourceKind::UnsynchronizedPool(PoolOptions)` |

Both strategies are wrapped in a `GuardedUpstream` that controls what happens
when the resource runs out of memory (see [Hermetic mode](#hermetic-mode) below).

---

## Hermetic mode

Both resource kinds accept a `hermetic` flag that determines the behaviour when
the resource can no longer serve an allocation from its own storage:

| `hermetic` | Overflow behaviour |
|---|---|
| `true` | Logs a **fatal** message (via `score::mw::log`) and calls `std::abort()`. No allocation ever escapes to the system heap. |
| `false` | Logs a **warning** and delegates to `score::cpp::pmr::new_delete_resource()` (the system heap). |

Hermetic mode is intended for safety-sensitive contexts where latency guarantees
or memory-isolation requirements forbid runtime heap use after start-up.

---

## Usage

### C FFI API

The C++ side exposes a plain C interface (`cpp/ffi.h`) that is consumed both by
the Rust crate and directly from C++ tests:

```c
void* memres_new_monotonic(size_t size, bool hermetic);
void* memres_new_unsynchronized_pool(
    size_t max_blocks_per_chunk,      // 0 = implementation default
    size_t largest_required_pool_block, // 0 = implementation default
    bool hermetic);
void* memres_new_default();
void  memres_destroy(void* handle);
void* memres_as_memory_resource(void* handle);
```

All functions are `noexcept`.  `memres_destroy(nullptr)` is a no-op.  The
pointer returned by `memres_as_memory_resource` is valid until `memres_destroy`
is called on the same handle.

### Rust API

```rust
use memres::{MemoryResource, PoolOptions, ResourceKind};

// Monotonic buffer of 1024 bytes; aborts on overflow (hermetic).
let mr = MemoryResource::new(ResourceKind::Monotonic { size: 1024 }, true);

// Pool resource with implementation-chosen defaults; falls back to the heap.
let mr = MemoryResource::new(
    ResourceKind::UnsynchronizedPool(PoolOptions::default()),
    false,
);

// Pass `ptr.as_ptr()` to any C++ function that accepts
// `score::cpp::pmr::memory_resource*`.
let ptr = mr.as_memory_resource_ptr();
some_cpp_function(ptr.as_ptr());

// `ptr` (and its borrow of `mr`) are dropped before `mr` goes out of scope.
// `mr` drops here; the C++ resource and all its storage are freed.
```

`MemoryResource::as_memory_resource_ptr()` returns a `MemoryResourcePtr<'_>`
that borrows `self` for the duration of the binding.  The borrow checker
prevents `mr` from being moved or dropped while the pointer is live, making
dangling-pointer use a **compile-time error** rather than undefined behaviour
at runtime.  Call `.as_ptr()` on the wrapper to obtain the raw
`*mut ScoreMemoryResource` to pass to C++.  Any C++ object whose storage was
allocated from that resource must be destroyed **before** `mr` is dropped.

### Thread safety

`MemoryResource<S>` carries a compile-time marker `S` (`Local` or `Shared`):

- `MemoryResource::new(...)` returns `MemoryResource<Local>` — the monotonic and
  pool resources are single-threaded, so the handle is `!Send + !Sync`.
- `MemoryResource::<Shared>::default_resource()` wraps
  `score::cpp::pmr::get_default_resource()`.  That resource forwards to the
  data-race-free global `operator new`/`operator delete`, so the handle is
  `Send + Sync`.

## Design

### Layer overview

```
┌─────────────────────────────────────────┐
│  Rust crate  (src/lib.rs)               │
│  MemoryResource, ResourceKind,          │
│  PoolOptions                            │
└──────────────────┬──────────────────────┘
                   │ extern "C" FFI
┌──────────────────▼──────────────────────┐
│  C FFI layer  (cpp/ffi.h / ffi.cpp)     │
│  memres_new_*  memres_destroy           │
│  memres_as_memory_resource              │
└──────────────────┬──────────────────────┘
                   │
┌──────────────────▼──────────────────────┐
│  Handle layer  (memory_resource_handle) │
│  MemResHandle (abstract base)           │
│  MonotonicHandle                        │
│  UnsynchronizedPoolHandle               │
│  DefaultHandle                          │
└──────────────────┬──────────────────────┘
                   │ upstream pointer
┌──────────────────▼──────────────────────┐
│  GuardedUpstream                        │
│  Intercepts overflow; aborts or warns   │
└─────────────────────────────────────────┘
```

### Handle ownership and destruction order

Each concrete handle class stores its members in a specific order to guarantee
safe destruction:

**`MonotonicHandle`**
```
upstream_  ← constructed first, destroyed last
buffer_    ← backing byte array; must outlive inner_
inner_     ← constructed last, destroyed first; calls upstream_ on overflow
```

**`UnsynchronizedPoolHandle`**
```
upstream_  ← constructed first, destroyed last
inner_     ← constructed last, destroyed first; releases blocks to upstream_ on destruction
```

C++ destroys members in reverse declaration order, so `inner_` is always torn
down before `upstream_`, ensuring all pool-release calls reach a still-valid
object.

### Critical lifetime invariant

Any object whose element storage was allocated from a `MemoryResource` (or the
equivalent C++ handle) **must be destroyed before** that `MemoryResource` is
destroyed.

In Rust this is enforced naturally by LIFO drop order for local variables.  In
C++ tests, ensure PMR-backed containers go out of scope (or are explicitly
destroyed) before calling `memres_destroy()`.  Violating this rule causes a
use-after-free when the container's destructor tries to return memory to an
already-freed pool.

---

## Bazel targets

| Target | Kind | Description |
|---|---|---|
| `:guarded_upstream` | `cc_library` | `GuardedUpstream` C++ class |
| `:memory_resource_handle` | `cc_library` | Handle class hierarchy |
| `:memres_cpp` | `cc_library` | C FFI (`ffi.h` / `ffi.cpp`); tagged `FFI` |
| `:memres` | `rust_library` | Rust crate |
| `:memres_test` | `cc_test` | C++ unit tests (GoogleTest) |
| `:memres_rust_test` | `rust_test` | Rust unit tests |

Run all tests:

```sh
bazel test //score/language/rust/memres/...
```

---

## Directory structure

```
memres/
├── BUILD                      Bazel build file
├── README.md                  This file
├── cpp/
│   ├── ffi.h / ffi.cpp        Plain-C API consumed by Rust and C++ callers
│   ├── guarded_upstream.h/.cpp  Overflow-interception memory resource
│   └── memory_resource_handle.h/.cpp  Concrete handle classes
├── example/
│   ├── main.rs                Rust binary demonstrating hermetic abort
│   ├── example_helpers.h/.cpp C++ helpers used by the example and Rust tests
│   └── BUILD
├── src/
│   └── lib.rs                 Rust crate root
└── test/
    ├── memres_test.cpp         C++ GoogleTest tests
    └── memres_rust_test.rs     Rust unit tests
```
