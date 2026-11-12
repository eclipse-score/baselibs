# Base Libraries Integration Example

This directory contains a minimal example demonstrating how to use S-CORE Base Libraries as an external Bazel module dependency.

## Purpose

This example serves as an integration test in CI to verify that Base Libraries:
- Can be consumed as a module by external projects
- Has correct transitive dependency declarations
- Exposes only intended public targets

## Building the Example

```bash
cd examples/integration
bazel build //...
```

### QNX Configurations

The `x86_64-qnx` and `aarch64-qnx` configs use QNX SDP 8.0.4 for local builds and tests.
CI uses the parallel `x86_64-qnx-sdp_8.0.5` and `aarch64-qnx-sdp_8.0.5` configs, which select both compiler and IFS toolchains from SDP 8.0.5.
Fetching SDP 8.0.5 requires a GitHub token with read access to `eclipse-score/qnx_sdp_pkg` (`GH_TOKEN` or `~/.netrc`).
The parallel configs are temporary and will be collapsed when SDP 8.0.5 becomes the default.

## Modifying the Example

To add additional base libraries features to test:

1. Add dependencies to the `cc_binary` target in `BUILD`
2. Include headers in `main.cpp`
3. Run `bazel build //...` to verify

Example adding JSON support:
```python
# BUILD
deps = [
    "@score_baselibs//score/result:result",
    "@score_baselibs//score/json:json",
],
```

```cpp
// main.cpp
#include "score/result/result.h"
#include "score/json/json.h"
```
