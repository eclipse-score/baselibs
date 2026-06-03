# Dynamic backend loading for mw::log

## Goal

- mw/log as a single shared library (.so) with console backend/sink built-in.
- backend .so files (libmwlog_file_backend.so, libmwlog_remote_backend.so, libmwlog_slog_backend.so) deployed to a well-known directory by the integrator. Each backend is an independent shared library.
- At runtime, mw/log discovers and loads all *backend.so files from the backend directory and registers the available backends.
- Libraries and binaries depend on a single mw/log target. No per-backend deps needed.
- Customer-provided backends (libmwlog_custom_backend.so) use the same directory based discovery mechanism.

## Targets

1. score_baselibs - libmwlog.so (cc_shared_library)

- frontend
- console backend (built-in)
- backend_table (gBackendCreators)
- backend_loader (via dl_open)

2. score_logging - *backend.so (cc_shared_library)

- libmwlog_file_backend.so
- libmwlog_remote_backend.so
- libmwlog_slog_backend.so (QNX)
- libmwlog_custom_backend.so (custom)

## Assessment

### Appoach Comparision for the intended use case

| Criterion | *dlopen() | **DT_NEEDED | Weak Symbols | Global Backend Table (static) - exisiting implemenation |
|-----------|----------------------|--------------------------|---------------------------|--------------------------------------|
| Graceful degradation | Yes - fallback chain (console to stub) | No - process fails if .so missing | Yes - weak defaults | Yes - console fallback for unavailable backends |
| Runtime dependency | Plugin .so must be on trusted filesystem | .so must be on loader search path | None beyond normal linking | None - all resolved at link time |
| Attack surface | Lower than single bundle - each process loads only the backends it needs; a file-only process never loads DataRouter deps | Medium - loader search paths | Lowest - no runtime loading | Lowest - no runtime loading, no filesystem deps |
| QNX OS compatibility | Supported (RTLD_NOSHARE available) | Supported | Supported but toolchain-dependent | Full - standard C++ |
| Deployment complexity | per-backend .so files deployed to well-known directory; directory-based scanning | Similar to dl_open | Lowest | Lowest - single binary |
| Customer plugin support | Full - customer ships .so without rebuilding | No | No | No - requires static linking |
| Implementation complexity | Requires explicit loader code (DynamicBackendLoader: dlopen, dlsym, version check, fallback chain) | Zero - runtime linker resolves everything automatically before main(); no application code needed | Requires weak symbol declarations and fallback definitions | Requires registration table and factory map |

*dlopen() - Is an imperative, runtime API. The application explicitly calls dlopen() in code, controls when and whether to load a library, inspects the return value, and can fall back gracefully on failure.

**DT_NEEDED - Is a declarative, link-time mechanism. The dependency is recorded in the ELF .dynamic section when you link with -lfoo. The runtime linker (ld.so / ldqnx.so) resolves all DT_NEEDED entries automatically before main() runs. If any listed .so is missing, the process aborts at startup - the application never gets a chance to handle the failure.

Since dlopen() fits best for all the criterias, the following sections details out the requirments for detailed design, implementation, verification and validation steps considering Safety, Security and performance aspects.

References:
- [POSIX dlopen](https://man7.org/linux/man-pages/man3/dlopen.3.html)
- [QNX dlopen](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html)
- [DT_NEEDED elf](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [QNX8 lazy loading](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.prog/topic/devel_Lazy_loading.html)

## Dynamic loading and Application Binary Interface (ABI)

Dynamic loading introduces an ABI boundary between the main binary and the backend plugin .so. Unlike static linking where the compiler and linker can verify type compatibility at build time, dynamic linking defers binding to runtime making ABI stability an explicit design concern.

mw::log is classified as ASIL-B. The frontend preserves freedom from interference (FFI) - dlopen() occurs only during init, and the post-init Recorder interface boundary is identical to the static-link case.

## Runtime loading of backends via dl_open()

mw::log would use dlopen() based dynamic loading to compose recorders and respective backends at runtime. A libmwlog.so (frontend + console backend + DynamicBackendLoader) is the sole build dependency for all consumers. Per-backend .so files (libmwlog_file_backend.so, libmwlog_remote_backend.so, libmwlog_slog_backend.so) are deployed by the integrator and loaded at runtime from a configuration-specified list. The plugin boundary uses a C++ interface - backend .so files export a C++ recorder factory function discovered via a minimal extern "C" entry point.

Architectural decision: This approach preserves the existing C++ backend implementation unchanged, at the cost of requiring toolchain identity between libmwlog.so and all backend .so files. Security is enforced via OS-level compensating controls (pathtrust, secpol, procnto -mX, setuid bit, configuration-driven loading) rather than libc isolation.

### Plugin API (mw_log_plugin_api.h)

The plugin API uses minimal extern "C" entry points: one for version checking and one for backend registration. Both symbols use C linkage to avoid name-mangling dependencies across toolchain versions.

```cpp
#ifdef __cplusplus
extern "C" {
#endif

#define MW_LOG_ABI_VERSION 1U

/// Returns the ABI version this plugin was built against.
uint32_t mw_log_version(void);

/// Registers the backend's recorder creator function into the global backend table.
/// Called by dynamic backend loader after mw_log_version check passes.
void mw_log_register_backends(void) noexcept;

#ifdef __cplusplus
}
#endif
```

Design rationale:
- Both mw_log_version() and mw_log_register_backends() use extern "C" to provide stable, mangling-independent symbols for dlsym() lookup.
- The backend calls RegisterBackend() from backend_table.h directly - the existing gBackendCreators[] table *is* the registry. No separate Registry object or C++ factory symbol is needed across the boundary.
- Shared libc/libstdc++ (no RTLD_NOSHARE) enables standard C++ types, smart pointers, and allocators to cross the dlopen() boundary safely within the registration and recorder creation path.
- Constraint: All backend .so files MUST be built with the identical toolchain (compiler version, -std= flag, libstdc++ version) as libmwlog.so. This is enforced via CI (REQ-DD-1).
- The no-dlclose() policy (REQ-SAFE-1) guarantees that all code pages and vtable pointers remain valid for process lifetime.

## 1. Architecture

### 1.1 Detailed Design

Covers requirements for Detailed Design (DD) and Implementation

#### REQ-DD-1: C++ Plugin Boundary with Toolchain Identity

The plugin .so files shall use the existing C++ Recorder interface. Each backend .so exports two extern "C" entry points: mw_log_version() (ABI version gate) and mw_log_register_backends() (registration callback). The registration callback calls RegisterBackend() from backend_table.h, which stores a RecorderCreatorFn function pointer. The RecorderCreatorFn returns std::unique_ptr<Recorder>. C++ types (std::unique_ptr, Configuration&, amp::pmr::memory_resource*) are permitted to cross the dlopen() boundary at recorder creation time. "RTLD_NOSHARE" shall not be used - all backend .so files share the process's libc and libstdc++ instances.

Toolchain identity constraint: All backend .so files and libmwlog.so shall be built with the identical toolchain: same compiler version, same -std=c++XX flag, same optimization level (-O2), and same libstdc++/libc++ version. A CI gate shall enforce this by comparing compiler identification hashes (E.g.: gcc --version fingerprint) embedded in each .so at build time.

Reasoning: score_baselibs and score_logging reside in separate Bazel workspaces but share the same score_toolchain and reference integration platform. Since RTLD_NOSHARE is not used, libc/libstdc++ are shared, making C++ types safe to pass across the boundary "within the same toolchain". The toolchain identity CI gate is the mechanism that prevents C++ ABI mismatches.

Limitation: Customer provided plugins built with a different toolchain are not supported.

References:
- [Itanium C++ ABI - Name Mangling & Virtual Table Layout](https://itanium-cxx-abi.github.io/cxx-abi/abi.html)
- [GCC ABI Policy and Guidelines](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html)
- [POSIX dlopen()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlopen.html)

#### REQ-DD-2: ABI Version Guard

Each plugin .so shall export an extern "C" function mw_log_version() returning MW_LOG_ABI_VERSION. The DynamicBackendLoader shall call mw_log_version() immediately after dlopen() and reject the plugin if the returned version does not match MW_LOG_ABI_VERSION compiled into libmwlog.so. MW_LOG_ABI_VERSION shall be incremented when the Recorder interface changes in a binary-incompatible way (virtual method addition, removal, reorder, or signature change).

Reasoning: The version check is a coarse-grained ABI gate. It catches obvious mismatches (E.g.: a backend built against an older Recorder interface). It does not catch subtle C++ ABI differences caused by toolchain mismatches - the toolchain identity CI gate (REQ-DD-1) is the primary defense for that class of errors.

References:
- [Semantic Versioning 2.0.0](https://semver.org/) - versioning contract for API/ABI changes.

#### REQ-DD-3: Configuration-Driven Backend Loading

The DynamicBackendLoader shall load only the backend .so files explicitly listed in the logging configuration file (E.g.: ecu_logging_config.json field "backends": ["libmwlog_file_backend.so", "libmwlog_remote_backend.so"]). It shall not blindly scan the backend directory for all matching files. Each listed filename is joined with the well-known backend directory path (compile-time default /opt/mwlog/backends/, overridable via config file - never via environment variable). If a listed file is absent, it is treated as "not deployed" (info-level diagnostic). If no configuration is provided, no backends are loaded and the system uses console-only logging.

Reasoning: Glob-based directory scanning creates a wider attack surface - a malicious .so matching the naming pattern placed in the backend directory (via a compromised build pipeline or packaging error) would be loaded automatically. Configuration-driven loading eliminates this vector and gives the integrator explicit control over which backends are loaded and in what order.

References:
- [CWE-426: Untrusted Search Path](https://cwe.mitre.org/data/definitions/426.html)

#### REQ-DD-4: Fallback Chain

The DynamicBackendLoader shall implement the following fallback chain:

1. For each configured backend: attempt dlopen(), ABI check via mw_log_version(), then call mw_log_register_backends().
2. If a configured backend fails to load (.so present but dlopen()/ABI check/registration fails): report an ERROR. This is an actionable deployment error.
3. If a configured backend .so is absent (file does not exist): report a warning - This is a normal "not deployed" condition.
4. If no recorders are registered after loading, fall back to console-only logging (built-in).
5. If console allocation also fails then EmptyRecorder (stub, no-op).

Fallback diagnostic messages shall use a non-mw::log channel (slogf() on QNX, stderr on Linux) to avoid circular dependency.

References:
- [AUTOSAR Adaptive Platform - R22-11, Log and Trace](https://www.autosar.org/fileadmin/standards/R22-11/FO/AUTOSAR_PRS_LogAndTraceProtocol.pdf)

#### REQ-DD-5: Frontend FFI Preservation

The frontend's steady-state behavior (after init) shall not be affected by which recorder backend is loaded. dlopen() shall occur only during the initialization window. Post-init, all logging calls go through the Recorder interface - identical to the static-link architecture. Backend failures shall not propagate into the frontend.

Reasoning: mw::log is ASIL B. ISO 26262: Part 6 requires freedom from interference (FFI). Confining dynamic loading to init preserves temporal, spatial, functional, and resource FFI. Take note of existing AoUs of mw::log that init is not time bounded.

#### REQ-DD-6: Shared libc/libstdc++ Contract

RTLD_NOSHARE is explicitly not used. All backend .so files share the process's single libc and libstdc++ instance. This means:

- errno is shared (thread-local, single instance per thread).
- malloc/free operate on a single heap - memory allocated by the backend can be freed by the frontend and vice versa.
- File descriptors, atexit() handlers, and signal state are process-global and shared.
- C++ standard library types (std::unique_ptr, std::string, std::mutex, etc.) have identical layouts and can safely cross the dlopen() boundary.

Constraints imposed by this decision:

1. Toolchain identity is mandatory (REQ-DD-1). Different libstdc++ versions have different std::string layouts (pre/post-C++11 ABI), different allocator internals, etc.
2. RTLD_NOSHARE cannot be enabled without first refactoring to a pure C ABI boundary. This is an explicit, documented trade-off.
3. Security relies on OS-level compensating controls (pathtrust, secpol, procnto -mX, setuid bit) rather than libc isolation (see Security Requirements).

Reasoning: The existing backend implementation (RemoteDltRecorderFactory, DataRouterRecorder, DatarouterMessageClientImpl) uses C++ standard library types, spawns threads, opens file descriptors, and maps shared memory. These operations require a shared libc/libstdc++ instance to function correctly. Isolating libc via RTLD_NOSHARE would require a complete refactor to a pure C ABI boundary - a huge refactoring effort given that OS-level integrity controls (qtsafefs, pathtrust, secpol) already prevent loading untrusted code.

References:
- [QNX 8.0 dlopen()](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html)
- [GCC ABI Policy and Guidelines](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html)

#### REQ-DD-7: Backend Loading inside Runtime Constructor (Meyer's Singleton)

LoadConfiguredBackends() shall be called inside Runtime::Runtime(), before RegistryAwareRecorderFactory::CreateFromConfiguration(). The execution sequence within the Runtime constructor shall be:

1. LoadConfiguredBackends() - writes to gBackendCreators[] via RegisterBackend().
2. CreateRecorderFactory() would CreateFromConfiguration() - reads gBackendCreators[] via CreateRecorderForMode().

This ordering guarantees that all writes to gBackendCreators[] complete before any reads, within the same function call on the same thread. No synchronization primitive (mutex, atomic) is required on gBackendCreators[] itself.

Thread-safety is provided by the C++11 Meyer's singleton guarantee: Runtime is constructed via singleton::MeyerSingleton<Runtime>::GetInstance(), which uses the C++11 "magic statics" guarantee ([stmt.dcl] p4) - if multiple threads call GetInstance() concurrently, exactly one thread executes the constructor while all others block until construction completes. After construction, gBackendCreators[] is effectively immutable for the remainder of the process lifetime.

Invariants:
- LoadConfiguredBackends() is called exactly once, inside Runtime::Runtime(), on the thread that first triggers singleton construction.
- All writes to gBackendCreators[] (both static-init registrants like console and dynamic-load registrants) complete before CreateFromConfiguration() reads the table.
- After Runtime::Runtime() returns, gBackendCreators[] shall not be modified.

Implementation note: Runtime::Runtime() must read the backend configuration (directory path, filenames) before calling LoadConfiguredBackends(). This requires splitting config reading into two phases: (1) read backend list for LoadConfiguredBackends(), (2) read full config for CreateFromConfiguration(). Alternatively, LoadConfiguredBackends() can read the backend list independently from the same config source.

References:
- [C++11 Standard - [stmt.dcl] p4](https://timsong-cpp.github.io/cppwp/n4659/stmt.dcl#3) - concurrent execution of block-scope static variable initialization.
- [C++17 Standard - [intro.races]](https://timsong-cpp.github.io/cppwp/n4659/intro.races) - data race definition and undefined behavior.
- [ISO 26262-6:2018 - Freedom from Interference](https://www.iso.org/standard/68391.html)

### 1.2 Verification Requirements

#### REQ-DD-V1: ABI Version Mismatch Rejection

Verify that the DynamicBackendLoader rejects a plugin .so whose mw_log_version() returns a value different from MW_LOG_ABI_VERSION. Verify the fallback chain activates and the mismatch is logged.

Verification method: Unit/Component test - mock plugin returning mismatched version; verify rejection, fallback, and diagnostic output.

#### REQ-DD-V2: Configuration-Driven Loading

Verify that the DynamicBackendLoader loads only the .so files listed in the configuration. Place an additional libmwlog_rogue_backend.so in the backend directory that is not listed in the configuration. Verify it is not loaded.

NOTE: Existing feature flags shall be deprecated and replaced with this configuration option. The integrator explicitly lists which backends to load in the configuration file. Having two overlapping control mechanisms (feature flags + backend selection in config) creates confusion and contradictory states.

Verification method: Integration test - deploy extra .so alongside configured ones; verify only configured backends are loaded and instantiated.

#### REQ-DD-V3: Fallback Chain Activation

Verify all fallback chain paths:
1. All configured backends load successfully and all registered.
2. One configured backend missing and remaining load
3. One configured backend corrupt/failing and remaining load; error logged.
4. All configured backends fail amd console-only fallback activates.
5. if Console allocation fails and EmptyRecorder stub is returned.

Verification method: Component test - simulate each failure scenario; verify correct fallback behavior and diagnostic output.

#### REQ-DD-V4: Backend Loading Order Inside Runtime Constructor

Ensure that LoadConfiguredBackends() executes before CreateFromConfiguration() within Runtime::Runtime(), and that gBackendCreators[] is not modified after construction:

1. Detailed Design constraint test: Load a backend plugin that registers under LogMode::kFile. Verify that Runtime::GetRecorder() returns a FileRecorder (not EmptyRecorder or TextRecorder), confirming that dynamic registration completed before the factory read the table.
2. TSan CI gate: Run the init sequence under ThreadSanitizer in a multi-threaded test (multiple threads calling LOG_INFO() concurrently during first init). Verify no data race reports on gBackendCreators[].
3. Immutability assertion: After Runtime::Runtime() completes, snapshot gBackendCreators[] and verify it does not change for the remainder of the test.

Verification method: Code review + unit/component test + TSan CI gate.

---

## 2. Safety Requirements

### 2.1 Detailed Design

#### REQ-SAFE-1: No dlclose() Policy

The loaded plugin handle shall not be closed during process lifetime. The handle is intentionally leaked: (void)handle;. This policy shall be enforced as follows:

1. The DynamicBackendLoader shall not store the dlopen() handle in any data structure that could be passed to dlclose().
2. The DynamicBackendLoader shall not expose any API that accepts or returns a dlopen() handle.
3. No code path in libmwlog.so or any backend .so shall call dlclose().

Verified invariant: The C++ virtual function table (vtable) and destructor code of Recorder objects returned by the backend factory reside in code pages mapped from the plugin .so. These code pages shall remain mapped for the entire process lifetime. This is the mechanism that prevents use-after-free (CWE-416) when the frontend destroys Recorder objects via std::unique_ptr<Recorder>.

Reasoning: If dlclose() is called, those code pages are unmapped - but Recorder objects remain alive with dangling vtable pointers. Any subsequent virtual call (StartRecord(), StopRecord(), Log()) would jump to unmapped memory, causing a segfault or silent corruption. Since the plugin is loaded once at init and recorders live for the entire process, there is no legitimate reason to unload.

NOTE: Adding dlclose() for "cleanup" (E.g.: in a shutdown path or test teardown) WILL introduce use-after-free. The no-dlclose policy is a safety invariant.

References:
- [CWE-416: Use After Free](https://cwe.mitre.org/data/definitions/416.html) - accessing memory through dangling pointers after dlclose() unmaps code pages.
- [POSIX dlclose()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlclose.html) - specifies that symbols from a closed library become undefined.

#### REQ-SAFE-2: Plugin Constructor Restrictions

The plugin .so shall not use mw::log in any __attribute__((constructor)) function or static initializer. The plugin .so shall not call setenv(), putenv(), signal(), or spawn threads in constructor functions.

Reasoning: Plugin constructors execute during dlopen() - before the version guard (mw_log_version()) is checked. A constructor calling mw::log creates a circular dependency and potential deadlock (the logging runtime may not yet be initialized). Constructors that modify global process state (setenv, signal handlers) can interfere with the frontend's FFI guarantees.

References:
- [POSIX dlopen()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlopen.html) - specifies that initialization routines (constructors) execute during dlopen() before returning the handle.
- [GCC Common Function Attributes - constructor](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html) - documents __attribute__((constructor)) execution semantics.
- [CWE-764: Multiple Locks](https://cwe.mitre.org/data/definitions/764.html) - deadlock risk from circular re-entrant locking during init.

#### REQ-SAFE-3: dlopen() and dlsym() Null-Check

All dlsym() return values shall be null-checked before use. If dlopen() returns nullptr, the error shall be logged (never via mw::log) and the fallback chain shall activate.

Reasoning: dlopen() can fail due to missing .so, corrupt binary, permission denied, or missing transitive dependencies. dlsym() can fail if the plugin does not export the expected symbol. Null-pointer dereference in the Runtime constructor would crash the process.

#### REQ-SAFE-4: FMEA Coverage for dlopen() Path

A formal FMEA entry shall be created covering:
- dlopen()
- dlsym()
- mw_log_version()
- mw_log_register_backends()

Each failure mode shall trace to a defined mitigation.

Reasoning: ISO 26262 Part 5 requires FMEA for all safety-relevant failure modes. The dlopen() path introduces new failure modes not present in the static-link architecture. In particular, mw_log_register_backends() executes plugin code within the frontend's address space - any failure in this function can corrupt the backend registration table or crash the process.

References:
- [ISO-26262](https://www.iso.org/standard/68391.html) - FMEA as a recommended analysis method for safety-relevant software.

#### REQ-SAFE-5: Pre-dlopen() ELF Section Version Marker (CI/Deploy-Time Gate)

Each plugin .so shall embed a version integer in a dedicated ELF section (E.g.: .note.mwlog_abi) at build time. This marker is intended for offline verification - it is not read at runtime by the DynamicBackendLoader.

CI/deploy-time enforcement: A CI gate or deployment validation script shall read the .note.mwlog_abi section from each backend .so (using readelf or objdump) and verify that the embedded version matches the MW_LOG_ABI_VERSION compiled into libmwlog.so. Backend .so files with mismatched versions shall not be deployed to the target filesystem.

This check catches version mismatches before deployment, preventing a scenario where dlopen() executes plugin constructors (REQ-SAFE-2) from an incompatible binary before the application can call mw_log_version(). The runtime mw_log_version() check (REQ-DD-2) remains the defense-in-depth mechanism for detecting mismatches that escape the CI gate.

Reasoning: dlopen() executes plugin constructors before the application can call mw_log_version(). A pre-deployment check avoids running any plugin code from an incompatible binary on the target. Runtime ELF section parsing was considered but rejected due to implementation complexity (requires ELF header parsing in application code) and marginal benefit over the deploy-time gate combined with pathtrust/qtsafefs as the primary protection against untrusted binaries.

References:
- [ELF Specification](https://refspecs.linuxfoundation.org/elf/elf.pdf) - .note section format for embedding metadata in ELF binaries.
- [NIST: Security and Privacy Engineering Principles](https://csrc.nist.gov/publications/detail/sp/800-53/rev-5/final)

```cpp
// Plugin side - static marker (readable offline via readelf/objdump, not loaded at runtime)
extern "C" {
    const uint32_t kMwLogAbiVersion __attribute__((section(".note.mwlog_abi"))) = 1U;
}
```

CI gate example:
```bash
#!/bin/bash
# Verify ABI version in backend .so matches libmwlog.so
EXPECTED_VERSION=$(readelf -x .note.mwlog_abi libmwlog.so | awk '...')
PLUGIN_VERSION=$(readelf -x .note.mwlog_abi "$1" | awk '...')
if [ "$EXPECTED_VERSION" != "$PLUGIN_VERSION" ]; then
    echo "FAIL: ABI version mismatch in $1 (expected $EXPECTED_VERSION, got $PLUGIN_VERSION)"
    exit 1
fi
```

### 2.2 Verification Requirements

#### REQ-SAFE-V1: dlopen() Failure Path Testing on Target ECU

All dlopen() failure paths shall be tested on the actual target ECU and QNX version:

| Test Case | Expected Behavior |
|-----------|-------------------|
| Missing .so file | Fallback chain activates; dlerror() output |
| Corrupt .so file | Graceful failure, no crash; error reported |
| Permission-denied .so | Fallback activates; error reported |
| First boot with flash storage | Worst-case dlopen() latency measured and documented |

Verification method: Target integration test - execute each scenario on the target ECU; verify behavior and capture timing.

#### REQ-SAFE-V2: PHM monitoring regressions

Verification method: Target integration test

#### REQ-SAFE-V3: Plugin ABI Mismatch Handling

Verify that a plugin built with a different MW_LOG_ABI_VERSION is rejected, the fallback chain activates correctly without undefined behavior.

Verification method: Unit test and Integration test - build a test plugin with MW_LOG_ABI_VERSION + 1; load it; verify rejection and fallback.

#### REQ-SAFE-V4: Stub Logging Detection

Verify that in a fully deployed production system, the EmptyRecorder stub is never the active recorder.

Verification method: Integration test (ITF) - deploy all configured backends; verify full recorder pipeline is active.

#### REQ-SAFE-V5: No mw::log Usage in Plugin Constructors

Verify that loading a plugin whose constructor calls mw::log does not deadlock or crash the init sequence.

Verification method: Manual Analysis.

---

## 3. Security Requirements

### 3.1 Detailed Design

#### REQ-SEC-1: Absolute Path for dlopen()

DynamicBackendLoader shall call dlopen() with a fully-qualified absolute path constructed by joining the backend directory path and the configured filename. Relative paths and bare filenames shall not be passed to dlopen().

Reasoning: Relative paths cause the runtime linker to search LD_LIBRARY_PATH and _CS_LIBPATH, which may point to untrusted directories. Absolute paths bypass the search entirely, eliminating CWE-426: Untrusted Search Path.

References:
- [CWE-426: Untrusted Search Path](https://cwe.mitre.org/data/definitions/426.html)
- [POSIX dlopen()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlopen.html) - specifies search order when a bare filename (no /) is passed.
- [QNX 8.0 dlopen() - Search Order](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html) - QNX-specific _CS_LIBPATH search behavior.

#### REQ-SEC-2: qtsafefs Enforcement

The backend directory shall reside on a qtsafefs-mounted filesystem. At init, the DynamicBackendLoader shall verify this by calling statvfs() on the backend directory and checking the filesystem type - not merely checking the path prefix.

Reasoning: qtsafefs provides integrity checking (hash verification) for all files on the filesystem. This is the primary protection against loading a tampered .so. A path-prefix check alone is insufficient - an attacker could mount a non-qtsafefs filesystem at the expected path.

References:
- [QNX 8.0 qtsafefs](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.qtsafefs/topic/qtsafefsd.html)

#### REQ-SEC-3: Compensating Controls for Omitted RTLD_NOSHARE

dlopen() shall use RTLD_NOW | RTLD_LOCAL (without RTLD_NOSHARE). The decision to omit RTLD_NOSHARE is justified by the following compensating controls, ALL of which shall be enforced:

| # | Compensating Control | Requirement | Threat Mitigated |
|---|---------------------|-------------|-----------------|
| 1 | pathtrust + qtsafefs | REQ-SEC-2, AOU-1, AOU-2 | Prevents loading tampered/untrusted .so files at OS level |
| 2 | secpol: PROCMGR_AID_UNTRUSTED_EXEC denied | REQ-SEC-8 | Makes pathtrust mandatory - dlopen() cannot bypass trust verification |
| 3 | setuid bit on process | REQ-SEC-8 | Runtime linker automatically unsets LD_PRELOAD, LD_LIBRARY_PATH before any user code |
| 4 | procnto -mX | REQ-SEC-9 | Prevents PROT_EXEC on files without +x - blocks loading from /dev/shmem, /tmp |
| 5 | Configuration-driven loading | REQ-DD-3 | Only explicitly listed .so files are loaded - no glob-scan, no auto-discovery |

Without RTLD_NOSHARE, the following attack is theoretically possible: If a compromised component calls dlopen("libmwlog_remote_backend.so", ...) with a relative path before DynamicBackendLoader runs, the linker may return the already-loaded (malicious) handle. With all five compensating controls in place, this attack requires a build-chain compromise of a pathtrust-verified binary on a qtsafefs filesystem - a significantly higher bar than the RTLD_NOSHARE scenario alone addresses.

Reasoning: RTLD_NOSHARE creates a private libc/libstdc++ copy for the plugin, making C++ types unusable across the boundary. Since the design choice is to preserve the existing C++ backend implementation (REQ-DD-1, REQ-DD-6), RTLD_NOSHARE is incompatible with the architecture. The five compensating controls collectively provide equivalent or stronger security guarantees for the threat model.

References:
- [CWE-427: Uncontrolled Search Path Element](https://cwe.mitre.org/data/definitions/427.html)
- [QNX 8.0 pathtrust](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.security.system/topic/manual/pathtrust.html)

#### REQ-SEC-4: RPATH Hardening (Plugin and Transitive Dependencies)

Each backend .so shall be built with:
- -Wl,--disable-new-dtags - ensures RPATH is used (not RUNPATH). RPATH is not overridden by LD_LIBRARY_PATH.
- An explicit, absolute RPATH pointing only to qtsafefs-backed library directories (E.g.: -Wl,-rpath,/opt/mwlog/lib). No $ORIGIN-relative paths.

Reasoning: When dlopen() loads a backend .so, the runtime linker resolves its DT_NEEDED transitive dependencies using the search order: RPATH -> LD_LIBRARY_PATH -> _CS_LIBPATH. If RUNPATH is used instead of RPATH, LD_LIBRARY_PATH takes precedence, allowing an attacker to inject transitive dependencies. Absolute RPATH with --disable-new-dtags ensures transitive deps are resolved only from trusted paths.

References:
- [ld.so(8) - Linux Dynamic Linker: RPATH vs RUNPATH](https://man7.org/linux/man-pages/man8/ld.so.8.html)
- [ELF Specification - Dynamic Section (DT_RPATH, DT_RUNPATH)](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [CWE-426: Untrusted Search Path](https://cwe.mitre.org/data/definitions/426.html)
- [QNX 8.0 dlopen _CS_LIBPATH](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html)

#### REQ-SEC-5: Exported Symbol Minimization

Each plugin .so shall export only mw_log_version and mw_log_register_backends (both extern "C"). All other symbols shall be hidden via -fvisibility=hidden and a linker version script:

```
{
    global:
        mw_log_version;
        mw_log_register_backends;
    local:
        *;
};
```

Reasoning: Every exported symbol is an attack surface and a potential source of symbol collision (CWE-114). Minimizing exports hides internal implementation, eliminates interposition risk, and reduces the ABI surface that must remain stable.

References:
- [GCC Wiki - Visibility](https://gcc.gnu.org/wiki/Visibility)
- [GNU ld - Version Scripts](https://sourceware.org/binutils/docs/ld/VERSION.html) - linker version script syntax for controlling symbol export.

#### REQ-SEC-6: Environment Variable Integrity Check (Needs further investigation)

At init, the DynamicBackendLoader should snapshot the values of LD_PRELOAD, LD_LIBRARY_PATH, DL_DEBUG, and LD_DEBUG. Before each dlopen() call, it should verify these values have not changed. If any have been modified, a critical diagnostic shall be logged.

Known limitation (TOCTOU): This check is subject to a time-of-check time-of-use race condition. A concurrent thread calling setenv("LD_LIBRARY_PATH", "/attacker/path") between the snapshot verification and the dlopen() call can still succeed. This check is therefore a diagnostic aid, not a reliable security control. It MUST not be relied upon as the sole mitigation for environment variable manipulation.

The primary mitigation is the setuid bit (REQ-SEC-8), which causes the runtime linker to automatically unset LD_LIBRARY_PATH, LD_PRELOAD, DL_DEBUG, and LD_DEBUG before any user code runs - including library constructors. REQ-SEC-8 eliminates the TOCTOU race entirely because the variables are never present in the process environment.

Reasoning: On QNX 8.0, LD_LIBRARY_PATH is re-read on every dlopen() call. The snapshot-and-recheck approach provides observability (detecting unexpected environment mutations) but cannot guarantee atomicity with respect to dlopen(). The setuid bit (REQ-SEC-8) is the authoritative control.

References:
- [CWE-367: Time-of-check Time-of-use (TOCTOU) Race Condition](https://cwe.mitre.org/data/definitions/367.html) - race between environment variable check and dlopen() usage.
- [QNX 8.0 dlopen() - Environment Variable Handling](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html) - QNX re-reads LD_LIBRARY_PATH on each dlopen() call.
- [CWE-426: Untrusted Search Path](https://cwe.mitre.org/data/definitions/426.html) - LD_LIBRARY_PATH manipulation.

```cpp
void AssertCleanEnvironment() noexcept {
    const char* vars[] = {"LD_PRELOAD", "LD_LIBRARY_PATH", "DL_DEBUG", "LD_DEBUG"};
    for (const char* var : vars) {
        const char* val = std::getenv(var);
        if (val != nullptr) {
            slogf(_SLOG_SETCODE(0, 0), _SLOG_CRITICAL,
                  "mw::log: %s is set - potential security risk", var); // slog buffers
        }
    }
}
```

#### REQ-SEC-7: dlerror() Output Sanitization

dlerror() strings shall be sanitized before logging. They shall be logged at internal/verbose level only, never at a level accessible to unprivileged log consumers.

Reasoning: dlerror() may return filesystem paths and internal details that reveal system layout to an attacker with log access.

References:
- [POSIX dlerror()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlerror.html) - specifies that dlerror() returns a human-readable string describing the last error.

#### REQ-SEC-8: QNX secpol and Ability Restrictions

Processes using mw::log with dynamic loading shall have PROCMGR_AID_UNTRUSTED_EXEC denied via secpol. This ensures only pathtrust-verified libraries can be loaded by dlopen(). The process should run with the "setuid" bit set, causing the runtime linker to automatically unset LD_LIBRARY_PATH, LD_PRELOAD, DL_DEBUG, and LD_DEBUG before any user code runs.

Reasoning: pathtrust is the OS-level gate that prevents dlopen() from loading untrusted binaries. Denying PROCMGR_AID_UNTRUSTED_EXEC makes this gate mandatory. The setuid bit provides automatic environment variable sanitization by the runtime linker - a stronger guarantee than application-level checks (REQ-SEC-6) because it runs before any user code, including library constructors.

References:
- [QNX 8.0 PROCMGR_AID_UNTRUSTED_EXEC](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/p/procmgr_ability.html)
- [POSIX setuid behavior - Runtime Linker](https://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html)

#### REQ-SEC-9: PROT_EXEC Enforcement via procnto -mX

Deployment shall require procnto -mX on the target ECU. All backend .so files shall have the +x (executable) permission bit set.

Reasoning: When procnto is started with -mX, it only allows PROT_EXEC on mmap() if the file has the +x bit set. This prevents loading .so files from writable directories like /dev/shmem or /tmp where an attacker could write a malicious library but would lack the ability to set the +x bit on a properly configured system.

References:
- [QNX 8.0 procnto - -mX Option](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.utilities/topic/p/procnto.html) - PROT_EXEC enforcement via execute permission bit.

#### REQ-SEC-10: ELF Hardening Flags

All backend .so files and libmwlog.so shall be built with:

| Flag | Purpose |
|------|---------|
| -Wl,-z,now | Full RELRO - all GOT entries resolved at load time, GOT marked read-only |
| -Wl,-z,relro | Partial RELRO - relocatable sections before GOT marked read-only |
| -Wl,-z,noexecstack | Non-executable stack |
| -fstack-protector-strong | Stack canary protection |
| -fPIC | Position-independent code |

Reasoning: Standard ELF hardening per Red Hat security hardening guidelines. Full RELRO prevents GOT overwrite attacks. Non-executable stack mitigates stack-based code injection. These are baseline requirements for any shared library in a safety-critical system.

References:
- [OWASP C-Based Toolchain Hardening Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/C-Based_Toolchain_Hardening_Cheat_Sheet.html) - comprehensive guide to compiler and linker hardening flags.
- [Red Hat Enterprise Linux - Security Hardening Guide](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/9/html/security_hardening/) - ELF hardening best practices.
- [ELF Specification - GNU_RELRO Segment](https://refspecs.linuxfoundation.org/elf/elf.pdf) - read-only relocation segment format.
- [GCC - -fstack-protector-strong](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html) - stack canary instrumentation for buffer overflow detection.

#### REQ-SEC-11: Backend Directory Ownership Verification

At init, the DynamicBackendLoader shall stat() the backend directory and verify:
- Owned by root or the expected system user.
- Not world-writable.

If either check fails, a critical warning shall be logged.

Reasoning: On qtsafefs, the filesystem is read-only and integrity-checked. On non-qtsafefs deployments (E.g.: development environments), a world-writable backend directory would allow any process to inject a malicious plugin.

References:
- [CWE-732: Incorrect Permission Assignment for Critical Resource](https://cwe.mitre.org/data/definitions/732.html) - world-writable directories allowing unauthorized file creation.
- [CWE-427: Uncontrolled Search Path Element](https://cwe.mitre.org/data/definitions/427.html) - plugin injection via writable plugin directories.
- [NIST SP 800-53 Rev. 5 - AC-6: Least Privilege](https://csrc.nist.gov/publications/detail/sp/800-53/rev-5/final) - principle of least privilege for file system permissions.
- [POSIX stat()](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)

### 3.2 Verification Requirements

#### REQ-SEC-V1: Absolute Path Enforcement

Verify that DynamicBackendLoader rejects any attempt to load a plugin via a relative path or bare filename.

Verification method: Unit test - attempt dlopen() with relative path and bare filename; verify rejection and error log.

#### REQ-SEC-V2: qtsafefs Mount Verification

Verify that DynamicBackendLoader rejects a backend directory that is not on a qtsafefs filesystem.

Verification method: Integration test (ITF) - mount a non-qtsafefs directory at the backend path; verify rejection and fallback to console.

#### REQ-SEC-V3: Compensating Controls Enforcement

Verify that all five compensating controls (REQ-SEC-3) are active and correctly prevent unauthorized library loading:

1. pathtrust: Place a valid .so on a non-pathtrust filesystem would verify dlopen() fails.
2. secpol: Verify process has PROCMGR_AID_UNTRUSTED_EXEC denied would untrusted .so cannot load.
3. setuid: Verify LD_LIBRARY_PATH and LD_PRELOAD are unset by runtime linker before main().
4. procnto -mX: Remove +x from a .so would verify dlopen() fails.
5. Config-driven: Place an extra libmwlog_hacky_backend.so in the backend directory (not in config) would verify it is not loaded.

Verification method: Target integration test (ITF) on QNX - execute each scenario; verify correct rejection and fallback chain activation.

#### REQ-SEC-V4: Exported Symbol Check

Verify that each backend .so exports only mw_log_version and mw_log_register_backends (both extern "C"). All other symbols shall be hidden.

Verification method: CI gate - nm --defined-only --extern-only on each libmwlog_*_backend.so; fail if unexpected symbols are found beyond mw_log_version and mw_log_register_backends.

#### REQ-SEC-V5: RPATH Verification

Verify that each backend .so has RPATH (not RUNPATH) containing only approved absolute paths to qtsafefs-backed directories.

Verification method: CI gate - readelf -d on each backend .so; verify RPATH contains only approved paths and no RUNPATH entries exist.

#### REQ-SEC-V6: ELF Hardening Verification

Verify that all .so files have GNU_RELRO segment, GNU_STACK without execute flag, and full RELRO binding.

Verification method: CI gate - readelf -l and readelf -d on each .so; verify GNU_RELRO present, GNU_STACK flags do not include E, and BIND_NOW is set.

#### REQ-SEC-V7: pathtrust Enforcement

Verify that dlopen() fails to load a .so from a non-pathtrust filesystem and that the fallback chain activates correctly.

Verification method: Integration test (ITF) on QNX - place a valid .so on a non-pathtrust filesystem; verify dlopen() fails and console fallback activates.

#### REQ-SEC-V8: Execute Permission Enforcement

Verify that a backend .so without the +x permission bit fails to load under procnto -mX.

Verification method: Target integration test (ITF) on QNX - remove +x from a .so; verify dlopen() fails and fallback chain activates.

#### REQ-SEC-V9: Environment Variable Mutation Detection

Verify that DynamicBackendLoader detects a change to LD_LIBRARY_PATH between init and dlopen() and aborts loading.

Verification method: Component test - modify LD_LIBRARY_PATH via setenv() after init snapshot; trigger dlopen(); verify load is aborted and critical diagnostic is logged.

#### REQ-SEC-V10: dlerror() Sanitization

Verify that dlerror() output logged by DynamicBackendLoader does not contain raw filesystem paths at user-accessible log levels.

Verification method: Unit test - force a dlopen() failure; capture logged output; verify no raw paths at non-internal log levels.

#### REQ-SEC-V11: Double-Load Same Soname from Different Paths

Empirically determine QNX 8.0 behavior when loading the same soname from two different absolute paths. Document results.

Verification method: Target integration test (ITF) on QNX - load two .so files with identical sonames from different paths (without RTLD_NOSHARE); document whether both are loaded, whether symbol conflicts occur, and whether the linker returns the first loaded handle or loads a fresh copy.

---

## 4. Performance Requirements

### 4.1 Detailed Design

#### REQ-PERF-1: Zero Steady-State Overhead

After initialization, the dynamic-loading architecture shall introduce zero additional overhead compared to the static-link approach. The recorder factory is created once during init; all subsequent logging calls go through the same Recorder interface via virtual function dispatch - identical to the static-link architecture.

Reasoning: The backend's Recorder object is held via std::unique_ptr<Recorder> - the same ownership model as the static-link case. No additional wrappers, adapters, or indirection layers are introduced. The dlopen() boundary is crossed only during init.

#### REQ-PERF-2: dlopen() with RTLD_NOW

Backend .so files shall be loaded with RTLD_NOW (immediate symbol resolution) rather than RTLD_LAZY.

Reasoning: RTLD_NOW performs all symbol resolution at load time, exposing any missing symbol errors immediately during init. RTLD_LAZY defers resolution to first use, which could cause a crash on the first record() call - unacceptable for an ASIL B component. The latency cost (2–15 ms per .so warm, up to 50–150 ms cold boot) is paid once during init, which is already covered as unbounded (Restriction #1).

References:
- [POSIX dlopen() - RTLD_NOW vs RTLD_LAZY](https://pubs.opengroup.org/onlinepubs/9699919799/functions/dlopen.html) - specifies immediate vs. deferred symbol resolution semantics.

### 4.2 Verification Requirements

#### REQ-PERF-V1: dlopen() Latency Profiling on Target ECU

Measure dlopen() latency on the target ECU for each per-backend .so file (file, remote, slog) under:
- Warm cache conditions
- Cold boot with flash storage (RTLD_NOW - full symbol resolution including all transitive dependencies)

Document measured values alongside PHM timing budgets with margin.

| Backend | Expected warm (ms) | Expected cold (ms) |
|---------|-------------------|-------------------|
| libmwlog_file_backend.so |  |  |
| libmwlog_remote_backend.so |  |  |
| libmwlog_slog_backend.so |  |  |

Verification method: Target profiling - measure dlopen() wall-clock time on the target ECU across 10+ iterations; report min/max/median/p99.

#### REQ-PERF-V2: Memory Footprint Comparison (Optional)

Measure the .text + .rodata + RSS footprint for processes in the following configurations and compare against the previous static-link architecture:

| Configuration | Expected footprint |
|---------------|-------------------|
| Console-only (no plugins) |  |
| File-only |  |
| File + remote |  |
| All backends (previous static-link) |  |

Verification method: Target measurement - use size on the target toolchain and /proc/<pid>/maps for runtime validation.

#### REQ-PERF-V3: Memory Footprint Improvement Validation (Optional)

Measure the .text + .rodata footprint for processes that load only the backends they need (console-only apps no longer link remote/file backend code) and verify the expected memory savings vs. static-link.

Verification method: Target measurement - compare process RSS between static-link (all backends always linked) and dynamic-load (only configured backends loaded) configurations.

#### REQ-PERF-V4: Toolchain Identity CI Gate Performance (Optional)

Verify that the toolchain identity CI gate (REQ-DD-1) does not add measurable overhead to the build pipeline (< 5 seconds per backend .so verification).

Verification method: CI timing measurement - record elapsed time of toolchain hash comparison step across 10+ builds.

---

## 5. Assumptions of Use (AoUs)

These requirements define the operational context and constraints that the integrator, deployer, and plugin author must satisfy for the dynamic loading architecture to function correctly and safely.

#### AOU-1: qtsafefs Backend Directory

The backend directory (default /opt/mwlog/backends/) shall reside on a qtsafefs-mounted filesystem with integrity checking enabled.

#### AOU-2: pathtrust-Enabled Filesystems

All filesystems from which backend .so files and their transitive dependencies are loaded shall have pathtrust enabled.

#### AOU-3: procnto -mX Enabled

The target ECU shall run procnto with the -mX flag to enforce PROT_EXEC restrictions on mmap(). Only files with the +x permission bit can be loaded as executable code.

#### AOU-4: secpol Configuration

The process's secpol configuration shall deny PROCMGR_AID_UNTRUSTED_EXEC. This ensures dlopen() only loads pathtrust-verified libraries.

#### AOU-5: Backend .so File Permissions

All deployed backend .so files shall:
- Have the +x (executable) permission bit set.
- Be owned by root (or the designated system user).
- Not be world-writable.

#### AOU-6: Environment Variable Integrity

LD_PRELOAD, LD_LIBRARY_PATH, DL_DEBUG, LD_DEBUG, and PATH shall not be modified after process start. The process should use the setuid bit to have the runtime linker automatically unset LD_LIBRARY_PATH and LD_PRELOAD.

#### AOU-7: Logging Configuration File

The logging configuration file (E.g.: ecu_logging_config.json) shall explicitly list the backend .so filenames to load. It shall reside on a qtsafefs-mounted filesystem to prevent tampering.

#### AOU-8: No mw::log in Plugin Constructors

Plugin .so files shall not call mw::log APIs in __attribute__((constructor)) functions or static initializers. Plugin constructors shall not call setenv(), putenv(), signal(), or spawn threads.

#### AOU-9: Shared Memory Semantics

Since RTLD_NOSHARE is not used, libc/libstdc++ are shared between libmwlog.so and all backend .so files. Memory allocated by the backend (E.g.: via std::make_unique) can be freed by the frontend and vice versa. Standard C++ ownership semantics (std::unique_ptr<Recorder>) apply normally across the dlopen() boundary. The backend's Recorder destructor is the primary cleanup mechanism.

#### AOU-10: Plugin Vtable Lifetime

The C++ virtual function table (vtable) of Recorder objects returned by the backend factory remains valid for the entire process lifetime. This is guaranteed by the no-dlclose() policy (REQ-SAFE-1) - the backend .so's code pages are never unmapped.

#### AOU-11: Toolchain Identity for Backend Plugins

All backend .so files shall be built with the identical toolchain as libmwlog.so:
- Same GCC/Clang version
- Same -std=c++XX flag
- Same libstdc++/libc++ version
- Same target architecture flags

Customer-provided backend plugins are not supported in this architecture. If customer plugins need to be supported, then these AoUs must be adhered to.
Backend plugins shall:
- Export mw_log_version (extern "C") returning MW_LOG_ABI_VERSION.
- Export mw_log_register_backends (extern "C", noexcept) which calls RegisterBackend() to register the backend's RecorderCreatorFn.
- Implement the Recorder interface as non-blocking and non-throwing on the hot path.
- Not depend on libc state isolation (since RTLD_NOSHARE is not used).

#### AOU-12: Transitive Dependencies on Trusted Filesystems

All shared libraries transitively required by backend .so files (i.e., their DT_NEEDED entries) shall reside on qtsafefs-mounted, pathtrust-enabled filesystems.

#### AOU-13: EM/PHM Timing Budget

The integrator shall ensure that the EM and PHM checkpoint timing budget for init sequences that include dynamic plugin loading accounts for the measured worst-case dlopen() latency (including cold-boot with RTLD_NOW) with margin.

## TODOs

1. The security requirements heavily focus on QNX (qtsafefs, pathtrust, secpol, procnto). Document security postures for Linux-based targets or development environments.
2. Measure memory footprint per application (static-link vs. dynamic).
3. Measure dlopen() latency on target ECU flash.
4. Investigate what Linux/QNX 8.0 does if we load the same soname from two different absolute paths.
5. Documentation on handling updates to per-backend .so files.
6. Document EM/PHM checkpoint timing budgets apply to the init sequence.
7. Document abilities (secpol) required for processes additionally.
