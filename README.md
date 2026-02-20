# Base Libraries

Foundational C++ libraries providing common functionality for S-CORE modules, from QM to ASIL-B integrity levels.

## Documentation

- **[Base Libraries Feature Documentation](https://eclipse-score.github.io/score/main/features/baselibs/index.html)**: High-level feature overview.
- **[Base Libraries Module Documentation](https://eclipse-score.github.io/score/main/modules/baselibs/index.html)**: Detailed module documentation, including architecture and component requirements.
- **Detailed Design**: Each library includes a `README.md` with usage examples and API details in its respective `score/` subdirectory.

## Using Base Libraries

To depend on `score_baselibs` in your Bazel module, add the following to your `MODULE.bazel` file:

```python
bazel_dep(name="score_baselibs", version="x.x.x")
```

Replace `x.x.x` with a version from the [S-CORE Bazel Registry](https://github.com/eclipse-score/bazel_registry/tree/main/modules/score_baselibs), where all available releases are listed.

### Required Configuration Options

To use Base Libraries properly, you need to set the following build settings in your `.bazelrc` file or pass them to the `bazel` command:

```bash
--@score_baselibs//score/json:base_library=nlohmann
--@score_baselibs//score/memory/shared/flags:use_typedshmd=False
```

### Using Unreleased Versions

To depend on an unreleased version (e.g., for testing or development), you can use a [non-registry override](https://bazel.build/external/module#non-registry_overrides) in your `MODULE.bazel` file, for example `git_override`:

```python
git_override(
    module_name = "score_baselibs",
    commit = "de5bb275ee9957433b3587582ac07d3eeb98dd6a",
    remote = "https://github.com/eclipse-score/baselibs.git",
)
```

Replace the `commit` value with the specific git hash you want to use. This override allows you to depend on any commit from the repository, bypassing the registry resolution.

## Contributing

### Prerequisites

To set up your development environment, follow the official [S-CORE Development Environment Guide](https://eclipse-score.github.io/score/main/contribute/development/development_environment.html). Key requirements include:

- **Operating System**: Linux host or a [DevContainer](https://containers.dev/) setup (see below).
- **Build System**: Bazel 8+ (managed via [Bazelisk](https://bazel.build/install/bazelisk)).
- **QNX 8.0 SDP**: For QNX builds, follow the setup instructions in [Bazel toolchains for QNX](https://github.com/eclipse-score/toolchains_qnx/tree/main?tab=readme-ov-file#where-to-obtain-the-qnx-80-sdp).

### DevContainer Setup (Optional)

This repository offers a [DevContainer](https://containers.dev/).

For setup instructions and enabling code completion, see [eclipse-score/devcontainer/README.md#inside-the-container](https://github.com/eclipse-score/devcontainer/blob/main/README.md#inside-the-container).

> [!NOTE]
> If you are using Docker on Windows **without `WSL2`** in between, you have to select the alternative container `eclipse-s-core-docker-on-windows`.

### Building the Module

First, clone the repository:
```bash
git clone git@github.com:eclipse-score/baselibs.git
cd baselibs
```

#### Building for x86_64 Linux

```bash
# Build all targets
bazel build --config=bl-x86_64-linux //...

# Run all tests
bazel test --config=bl-x86_64-linux //...
```

#### Building for AArch64 Linux

Use the same command as for x86_64 Linux, but replace `--config=bl-x86_64-linux` with `--config=bl-aarch64-linux`.

> [!NOTE]
> AArch64 Linux support is currently experimental. Some targets may not build successfully.

#### Executing Cross-Compiled Tests with QEMU

When cross-compiling for AArch64, test binaries cannot run natively on an x86_64 host. They are executed locally using [QEMU user-mode emulation](https://www.qemu.org/docs/master/user/main.html).

**Prerequisite:** Install the `qemu-user` package on your host system:

```bash
sudo apt install qemu-user # Debian/Ubuntu
```

Verify the installation:

```bash
qemu-aarch64 --version
```

Running tests:

```bash
bazel test --config=bl-aarch64-linux //score/...
```

#### Building for x86_64 QNX 8.0 SDP

```bash
# Build all targets
bazel build --config=bl-x86_64-qnx -- //score/...
```

#### Building for AArch64 QNX 8.0 SDP

Use the same command as for x86_64 QNX, but replace `--config=bl-x86_64-qnx` with `--config=bl-aarch64-qnx`.

### Sanitizers

To detect memory errors, undefined behavior, and memory leaks, run tests with sanitizers enabled:

```bash
bazel test --config=bl-x86_64-linux --config=asan_ubsan_lsan --build_tests_only -- //score/...
```

This configuration enables:
- **AddressSanitizer (ASan)**: Fast memory error detector.
- **UndefinedBehaviorSanitizer (UBSan)**: Detects undefined behavior at runtime.
- **LeakSanitizer (LSan)**: Memory leak detector.

The sanitizers are configured with verbose output and will halt on the first error detected.

### Generating Documentation

To generate the documentation locally:

```bash
bazel run //:docs
```

Access the documentation at `_build/index.html` in your browser.

## Support and Community

- **GitHub Issues**: [Report bugs or request features](https://github.com/eclipse-score/baselibs/issues).
- **GitHub Discussions**: [Ask questions and share ideas](https://github.com/orgs/eclipse-score/discussions/categories/base-libs-ft).
- **Communication and Coordination**: See the [pinned discussion](https://github.com/orgs/eclipse-score/discussions/1223) for details on communication channels and project coordination.
