# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

"""Wires the centralized S-CORE clang-tidy policy for this repository.

The S-CORE baseline `.clang-tidy` from `score_cpp_policies` is always applied
first. Every `.clang-tidy` file listed in `local_configs` below is handed to
clang-tidy as an available config, not forced via `--config-file`. This means
clang-tidy performs its own native per-directory search: for each file it
lints (including a target's own main source files, not just headers it
pulls in), it walks up from that file's directory and applies the nearest
`.clang-tidy` it finds among the declared files. The nearest config wins
outright; it does not merge with configs further up the tree unless it sets
`InheritParentConfig: true`.

To add a package-local override that clang-tidy actually picks up:

  1. Add `exports_files([".clang-tidy"])` to that package's own BUILD file.
     Bazel's lint actions run sandboxed with only their declared inputs
     visible, so an un-exported `.clang-tidy` sitting on disk is invisible to
     clang-tidy even though the file genuinely exists in the source tree.
  2. Create the `.clang-tidy` file in that package, following the same
     schema as `//:.clang-tidy`. Note that `Checks: '-*'` alone makes
     clang-tidy hard-error with "no checks enabled" (this repo's tooling
     never passes `--allow-no-checks`); to fully suppress diagnostics for a
     package without erroring, keep one real, unconfigured check enabled
     instead, e.g. `Checks: '-*,readability-identifier-naming'` with no
     matching `CheckOptions` entries, which is registered but never fires.
  3. Add `Label("//path/to/package:.clang-tidy")` to `local_configs` below.

Bazel's `glob()` cannot cross package boundaries, so there is no way to
auto-discover local configs without each package opting in via step 1.

`score/language/futurecpp` is such an override: its `.clang-tidy` is synced
from the upstream project via Copybara and kept as-is, since the package
mirrors upstream standard-library conventions rather than this repo's own
style.
"""

load("@score_cpp_policies//clang_tidy:defs.bzl", "make_clang_tidy_aspect", "make_clang_tidy_test")

clang_tidy_aspect = make_clang_tidy_aspect(
    binary = Label("@llvm_toolchain//:clang-tidy"),
    local_configs = [
        Label("//:.clang-tidy"),
        Label("//score/language/futurecpp:.clang-tidy"),
    ],
)

clang_tidy_test = make_clang_tidy_test(aspect = clang_tidy_aspect)
