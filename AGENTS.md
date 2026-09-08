# AGENTS.md

## Repository Overview

**score_baselibs** is a collection of foundational C++ and Rust libraries for the S-CORE (Eclipse) automotive platform, targeting ISO 26262 ASIL-B compliance with MISRA C++:2023. The C++ standard is **C++17**. Rust edition is **2021**. C++ exceptions are **not used** (they cause `abort` on target platforms). Supported platforms: **Linux** and **QNX 8.0 SDP**. Architectures: **x86_64** and **AArch64**. Build system: **Bazel 8 (bzlmod)**. The Bazel module name is `score_baselibs`.

## Repository Layout

```
score/          # C++ AND Rust libraries side by side (concurrency, containers, containers_rust, filesystem,
                # json, memory, network, os, result, allocator, pal, sync, thread, testing_macros, log_rust, etc.)
                # Most components have a docs/ subfolder with requirements, architecture, detailed design, and safety analysis.
src/            # Deprecated Bazel alias stubs. No real sources here.
examples/       # Integration and usage examples (C++ and Rust)
third_party/    # External dependency BUILD files (acl, openssl, libcap2, etc.)
docs/           # Feature-level and module-level Sphinx docs.
.github/        # CI workflows, CODEOWNERS, tools
```

**Key root files:** `MODULE.bazel` (Bazel module definition and deps), `.bazelrc` (all build configs), `Cargo.toml` (Rust workspace).

## Core Libraries and Preferred Alternatives

A few `score/` libraries exist specifically to replace a standard C++ feature that is banned, unavailable, or unsafe in this repository. Prefer these over the standard feature they replace:

- **`score/result`** — `score::Result<T>` is the repository's error-return type. Use it instead of C++ exceptions, which this repository does not use (see Repository Overview).
- **`score/filesystem`** — a C++ wrapper around POSIX filesystem operations, modeled on `std::filesystem` but with mockable/fake implementations for tests. Use it instead of the standard `<filesystem>` header, which might be not safety-certified on some platforms.
- **`score/language/futurecpp`** (namespace `score::cpp`) — backports of newer (C++20-and-later) standard library facilities to C++17, e.g. `score::cpp::jthread`, `score::cpp::span`, `score::cpp::expected`, `score::cpp::latch`, `score::cpp::stop_token`. Use these instead of the equivalent `std::` C++20+ types, since this repository is C++17 only. It also provides `score::cpp::pmr`, the repository's counterpart to the C++17 `std::pmr` polymorphic memory resource facilities (e.g. `score::cpp::pmr::polymorphic_allocator`, `score::cpp::pmr::memory_resource`, and the `score::cpp::pmr` container aliases like `vector`, `string`, `map`). Prefer `score::cpp::pmr` over `std::pmr`.
- **`score/os`** — the OS Abstraction Layer (OSAL): an adapter library giving an OS-independent interface to POSIX-like operating systems (Linux, QNX). Use it instead of calling raw OS/POSIX APIs directly (e.g. `SigAction` wraps `sigaction()`).
- **`libm`** (system library) — the C math library, for real-valued math functions. Use it instead of the C++ `<complex>`/`<complex.h>` headers, which might be not safety-certified on some platforms.

## Building and Testing

Always use `--config=bl-x86_64-linux` for Linux x86_64 builds. This flag is **required** — bare `bazel build //...` will fail due to missing platform/toolchain configuration.

### Build

```bash
bazel build --config=bl-x86_64-linux //score/...   # All C++ and Rust libraries
```

### Test

```bash
bazel test --config=bl-x86_64-linux //score/...    # All C++ and Rust tests
```

To test a single target, replace `//score/...` with the specific Bazel label, e.g., `//score/containers_rust:containers_test`.

### Other Configs (reference only — prefer bl-x86_64-linux)

- `--config=bl-aarch64-linux` — AArch64 Linux (cross-compile, needs `qemu-user`)
- `--config=bl-x86_64-qnx` / `--config=bl-aarch64-qnx` — QNX (needs SDP credentials). If a user requests a QNX build, first confirm they have QNX SDP credentials configured. If not, direct them to obtain credentials before proceeding and do not generate QNX build commands.

## Formatting

### Auto-fix all formatting (C++, Python, Rust, Starlark/BUILD, YAML)

```bash
bazel run //:format.fix
```

This formats C++ (clang-format), Python (ruff), Rust (rustfmt), Starlark/BUILD (buildifier), and YAML (yamlfmt).

To format only specific files instead of the whole project, list them after `--`:

```bash
bazel run //:format.fix -- score/result/error.cpp score/result/error.h
```

### Check formatting (without modifying files)

```bash
bazel test //:format.check
```

This runs all format checks (C++, Python, Rust, Starlark, YAML) as test targets.

### Copyright headers

Copyright headers are enforced for specific file extensions (`bazel`, `BUILD`, `bzl`, `c`, `cpp`, `h`, `hpp`, `ini`, `py`, `rs`, `rst`, `sh`, `yaml`, `yml`); CI will fail if a covered file's header is missing or incorrect.
If unsure how the header should look for a given file type, don't guess: run `copyright.fix` and let it generate the header for you.

```bash
bazel run //:copyright.check    # Verify
bazel run //:copyright.fix      # Auto-fix missing headers
```

**Every new file with a covered extension must have a copyright header.** The year must be the year in which the file is being created. If the current year is unknown, use `<YEAR>` as a placeholder.

C++ files (`.h`, `.cpp`):
```cpp
/********************************************************************************
 * Copyright (c) <YEAR> Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
```

Rust (`.rs`), Python (`.py`), Starlark (`.bzl`), BUILD files — use `#` comment prefix. Bazel files use the same text with `#` line comments.

## Linting

### clang-tidy (C++)

```bash
bazel build --config=bl-x86_64-linux --config=clang-tidy -- //...
```

Config: `.clang-tidy` at repo root.

Packages can also opt into a package-local `.clang-tidy` override, discovered by clang-tidy's
own per-directory config search (nearest directory wins). See the docstring in
`tools/lint/linters.bzl` for the exact steps and pitfalls (Bazel sandboxing requires the file
to be exported and registered; `Checks: '-*'` alone errors instead of disabling everything).
`score/language/futurecpp` uses this to keep linting under its own, Copybara-synced config
instead of this repo's stricter baseline.

### Rust clippy

```bash
bazel build --config=lint -- //score/...
```

### C++ Sanitizers

```bash
bazel test --config=bl-x86_64-linux --config=asan_ubsan_lsan --build_tests_only -- //score/...
```

## General coding commenting guidelines

- Write in English in complete sentences. Be concise.
- Avoid filler, pleasantries and hedging.
- Write one sentence per line.
- Do not decorate sections (e.g. with long ---------- comments).
- Wrap only when unavoidable, first try to rephrase the comment to fit. Respect formatter column limit.
- Do not wrap Markdown - rephrase or keep as is.
- Double spaces between sentences are unnecessary.

## C++ Coding Conventions

### Style (naming, files, formatting, comments)

**Before writing, modifying, or reviewing any C++, you MUST read [cpp-style-guide.md](./docs/cpp-style-guide.md)** and apply it on the first pass. It is the source of truth for naming, file/API structure, and comments. Do not rely on neighboring files as a style reference — some predate the guide and violate it. Mechanical formatting is applied by `.clang-format`.

C++ standard: C++17. No exceptions. No RTTI unless explicitly needed. Tests use Google Test (`cc_gtest_unit_test` macro from `bazel/unit_tests.bzl`), named `*_test.cpp` and colocated with source.

### Build Rules

- **Visibility:** Use `visibility = ["//visibility:public"]` only for libraries with a public API consumed by users of `score_baselibs`. Be conservative: do not expose internal or helper libraries. Internal targets should use package-level or narrower visibility.
- **Compiler warnings:** Use `features = COMPILER_WARNING_FEATURES` from `score/language/safecpp:toolchain_features.bzl`.

### Safety Tags

- **ASIL tags:** Use `tags = ["FFI"]` only for libraries targeting ASIL-B safety integrity level. QM-level libraries must not have this tag.

## Rust Conventions

- Do **not** run `rustfmt` directly. Use commands from Formatting section.
- Cargo workspace is defined in root `Cargo.toml`. Rust crates live under `score/`, alongside the C++ libraries.
- Cargo build requires Rust **1.90.0** (from `rust-toolchain.toml`). Bazel uses Ferrocene toolchain.
- Rust code should follow the Safety-Critical Rust Consortium coding guidelines: https://github.com/Safety-Critical-Rust-Consortium/safety-critical-rust-coding-guidelines/tree/main/src/coding-guidelines.
- Allowed Rust crates are listed in https://github.com/eclipse-score/score-crates/blob/main/MODULE.bazel. Adding new crate dependencies should be a last resort. If a required crate is missing and there is no meaningful alternative to implement a feature, consult the user before proceeding.

## Integration Test

The `examples/integration/` directory contains a module-level integration test. CI runs:

```bash
cd examples/integration && bazel build //...
```

## Documentation

Documentation uses Sphinx with the sphinx-needs extension, integrated with Bazel. The project follows the S-CORE process defined at https://github.com/eclipse-score/process_description. The metamodels for Sphinx-needs directives and Bazel integration are maintained at https://github.com/eclipse-score/docs-as-code.

`score/<component>/docs/` holds most components' documentation (requirements, architecture, detailed design, safety analysis), colocated with the component's source. `docs/` at the repo root hosts feature-level docs (`docs/features/`), module-level docs (`docs/module/`), and the docs of components not yet migrated to `score/<component>/docs/` (`docs/baselibs/components/<component>/docs/`).

```bash
bazel run //:docs    # Build docs, output at _build/index.html
```
