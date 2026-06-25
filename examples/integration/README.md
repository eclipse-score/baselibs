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

## Configuration

Required flags in `.bazelrc`:
- `--@score_baselibs//score/json:base_library=nlohmann`
- `--@score_baselibs//score/memory/shared/flags:use_typedshmd=False`

These must be set by consumers to properly configure Base Libraries.
