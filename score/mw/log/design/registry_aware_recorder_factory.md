## Table of Contents

1. [Overview](#1-overview)
2. [targets at a Glance](#2-targets-at-a-glance)
3. [PlantUML Diagrams](#4-plantuml-diagrams)
4. [How to Add a New Backend Registrant](#5-how-to-add-a-new-backend-registrant)
5. [Bazel Usage Guide](#6-bazel-usage-guide)
6. [Fallback Behavior](#7-fallback-behavior)
7. [Assumptions and Constraints](#8-assumptions-and-constraints)
8. [FAQs](#9-faqs)

## 1. Overview

The `RegistryAwareRecorderFactory` is the single `IRecorderFactory` implementation.
It provides a static plugin-based design that enables per-binary backend selection.

Motivation: A minimal stub backend / console is always made available.
Additional backends (console, file, remote DLT, slog2, custom) are
additive Bazel dependencies. Each backend self-registers into a
global function pointer table during static initialization. The factory queries this table
at runtime and gracefully falls back to console for any unavailable backend.

## 2. Targets at a Glance

> In order to verify locally, run:
> ```bash
>
> # All available backend plugins
> bazel query 'kind("cc_library", //score/mw/log/backend/...)' --output label
> NOTE: console backend is not found here since its provided by default as `log:console` verify deps via:
>
> bazel query 'deps(//platform/aas/mw/log:console, 1)' --output label
>
> # Full bundle (including all supported backends)
> bazel query 'deps(//platform/aas/mw/log:log, 1)' --output label
> ```

### Minimal target (always required)

| Bazel target | Provides |
|---|---|
| `//platform/aas/mw/log:minimal` | Frontend + stub backend — the minimum viable logging |
| `//platform/aas/mw/log:frontend` | `Logger`, `LogStream`, `Runtime` (public API) |

### Additive backend plugins (zero or more)

Each plugin uses `alwayslink = True` to ensure its static registrant is linked.

| Bazel target | Backend | Condition |
|---|---|---|
| `//platform/aas/mw/log:console` | Console logging | `config_KConsole_Logging` flag |
| `//score/mw/log/backend:file` | DLT file logging | `config_KFile_Logging` flag |
| `//score/mw/log/backend:remote` | DataRouter remote DLT | `Remote_Logging` flag |
| `//score/mw/log/backend:slog` | QNX slog2 | QNX only (`@platforms//os:qnx`) |

### Full bundle

| Bazel target | Includes |
|---|---|
| `//platform/aas/mw/log:log` | `:console` + `backend:file` + `backend:remote` + `backend:slog` (QNX only) |

Three guarantees:

1. Any deps on `mw/log:minimal` gets stub logging as default with additive backend capability.
2. Any deps on `mw/log:console` gets console logging as default with additive backend capability.
3. Additive composition: Adding `backend:file` doesn't conflict with backend `:console` or `backend:remote`.
4. Graceful fallback: Requesting a mode whose backend isn't linked falls back to console backend and if that fails to stub (`/dev/null`).

## 4. PlantUML Diagrams

| Diagram | Description |
|---------|-------------|
| Component Diagram | ![Component Diagram](./score/mw/log/design/backend_registration_component_diagram.puml) | Shows the layered architecture |
| Sequence Diagram | ![Sequence Diagram](./score/mw/log/design/backend_registration_sequence_diagram.puml) | Traces the full lifecycle from OS load through constant initialization, dynamic registration, first `LOG_xxx()` call, recorder creation with fallback logic, to shutdown. |
| Activity Diagram | ![Activity Diagram](./score/mw/log/design/rarf_activity_diagram.puml) | Details the decision flow including all fallback paths and error handling. |

## 5. How to Add a New Backend Registrant

### Step 1: Implement Your Recorder

Create a class inheriting from `Recorder` (all pure virtual `Log()` overloads, plus
`StartRecord()` and `StopRecord()`). Then create a recorder factory using the CRTP base:

```cpp
class MyRecorderFactory : public LogRecorderFactory<MyRecorderFactory>
{
  public:
    std::unique_ptr<Recorder> CreateConcreteLogRecorder(
        const Configuration& config,
        score::cpp::pmr::memory_resource* memory_resource);
};
```

### Step 2: Create the Registrant Source File

Create `score/mw/log/backend/my_registrant.cpp`:

```cpp
#include "score/mw/log/backend_table.h"
#include "path/to/my_recorder_factory.h"

namespace
{

std::unique_ptr<Recorder> CreateMyRecorder(
    const Configuration& config,
    score::cpp::pmr::memory_resource* memory_resource)
{
    MyRecorderFactory factory;
    return factory.CreateLogRecorder(config, memory_resource);
}

static const BackendRegistrant kMyRegistrant{LogMode::kCustom, &CreateMyRecorder};

}  // namespace
```

Key rules for the registrant:

- The creator function must match `RecorderCreatorFn` signature: `std::unique_ptr<Recorder>(const Configuration&, score::cpp::pmr::memory_resource*)`
- Use `static const` — the constructor runs during static initialization
- Choose the correct `LogMode` slot (or `kCustom` for custom backends)

### Step 3: Create the Bazel Target

Add to `score/mw/log/backend/BUILD`:

```python
cc_library(
    name = "my_backend",
    srcs = ["my_registrant.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "//platform/aas/mw/log:backend_table",
        "//path/to:my_recorder_factory",
    ],
    alwayslink = True,  # CRITICAL — ensures registration runs
)
```

> `alwayslink = True` is mandatory. Without it, the linker may discard the
> registrant's object file if no other symbol from it is referenced, and the backend
> will silently not register.

### Step 4: Wire Into Applications

Option A — All applications get it: Add your target to `//platform/aas/mw/log:log` deps.

Option B — Opt-in only: Document the dep. Application owners add:
```python
deps += ["//score/mw/log/backend:my_backend"]
```

## 6. Bazel Usage Guide

### Libraries

```python
cc_library(
    name = "my_library",
    deps = ["//platform/aas/mw/log:minimal"],
)
```

```python
cc_library(
    name = "my_library2",
    deps = ["//platform/aas/mw/log:console"],
)
```

### Service with file logging

```python
cc_binary(
    name = "my_service",
    deps = [
        "//score/mw/log/backend:file",      # + DLT file support
    ],
)
```

### Service with file + remote DLT

```python
cc_binary(
    name = "my_production_service",
    deps = [
        ":my_library2"                              # Includes console backend via deps
        "//score/mw/log/backend:file",       # + DLT file
        "//score/mw/log/backend:remote",     # + DataRouter remote DLT
    ],
)
```

### minimal target to support backwards compatibility

If your `cc_test` or `cc_binary` target deps on a library that deps only on the `mw/log:frontend`, then you must provide a concrete backend (`minimal`) as a minimum
and any additional backends if required. Otherwise you'll see an `error: undefined reference to 'score::mw::log::detail::CreateRecorderFactory()'`

```python
cc_library(
    name = "my_library",
    deps = ["//platform/aas/mw/log:frontend"],
)

cc_binary(
    name = "my_app",
    deps = [
        ":my_library",
        "//platform/aas/mw/log:minimal", # must be provided
    ],
)
```

### Full backend bundle

```python
cc_binary(
    name = "my_existing_app",
    deps = ["//platform/aas/mw/log:log"],  # All backends
)
```

## 7. Fallback Behavior

The `RegistryAwareRecorderFactory` implements a three-tier fallback chain:

```
Priority 1: Registered backend for requested LogMode
    (if not available) ->
Priority 2: Console backend (registered via log:console)
    (if console creation fails — should never happen) ->
Priority 3: EmptyRecorder (no-op, last resort)
```

## 8. Assumptions and Constraints

### Static Initialization Assumptions

| Assumption | Detail |
|------------|--------|
| `gBackendCreators` is constant-initialized | Guaranteed by C++ `[basic.start.static]/2` for `std::array` of trivial types with `{}` initialization |
| All registrants complete before `main()` | Guaranteed by C++ standard for objects with static storage duration |
| Each registrant writes a distinct slot | By design — each plugin uses a unique `LogMode` enum value |
| No registrant calls `LOG_xxx()` | Logging during static init is fragile and unsupported (existing constraint) - Read Assumptions of Use for mw::log |
| TU initialization order is unspecified | Not a concern because the target array is already valid (constant-initialized) when any registrant runs and fallback mechanism is in place |

### Thread Safety Assumptions

| Assumption | Detail |
|------------|--------|
| Registration is single-threaded | Static init happens before `main()`, single-threaded per C++ standard |
| Table reads are safe after `main()` | Table is immutable after dynamic init. No synchronization needed. |
| `Runtime` is not constructed during static init | The Meyers' singleton is created on first `LOG_xxx()` call, expected to be after `main()` |

## 9. FAQs

Q: What happens if two plugins register for the same `LogMode`?
A: The second registration overwrites the first. This is intentional — it allows
applications to override a default backend plugin. However, this scenario is unlikely
in practice since each `LogMode` has a canonical backend.

Q: Can a library add a backend plugin dep?
A: It *can*, but it *shouldn't*. Backend selection is a binary-level decision. Libraries
should depend on `//platform/aas/mw/log:console` (which provides the frontend API and
console default). The application binary should declare which additional backends it needs.

Q: What if I need logging during static initialization?
A: This is unsupported and fragile. The backend table may be only partially populated.
If `Runtime` is triggered before all registrants complete,
some backends may be missing. After `main()`, all backends are guaranteed to be available.
Moreover you would be violating the exisitng AoU (Assumptions of Use) for mw::log which
should be respected.

Q: How do I verify which backends are linked into my binary?
A: At runtime, call `IsBackendAvailable(mode)` for each `LogMode`. At build time, inspect
the transitive dependency closure of your `cc_binary` target via `bazel query`.
