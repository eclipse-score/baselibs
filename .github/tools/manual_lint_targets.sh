#!/usr/bin/env bash
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
# Finds `manual`-tagged targets that a linter aspect would otherwise apply
# to, and prints their labels one per line.
#
# `manual` only excludes a target from wildcards like `//...`; rules_lint's
# aspects don't check for it. Building the target by name is the only way to
# lint it, so the lint workflow appends this script's output to its build.
#
# docs()/docs_bundle() wrapper targets are excluded: their srcs are external
# repo code, nothing here to lint. Platform-incompatible targets are excluded
# too, since naming one explicitly fails the build instead of being skipped.
#
# Usage: manual_lint_targets.sh <bazel-build-config>
set -euo pipefail

config="${1:?usage: manual_lint_targets.sh <bazel-build-config>}"

# Rule kinds each linter aspect visits: clang-tidy (cc_*), Ruff (py_*),
# Clippy (rust_*).
lintable_kinds='cc_binary|cc_library|cc_test|py_binary|py_library|py_test|rust_binary|rust_library|rust_shared_library|rust_test'

# \b avoids matching tags that merely contain "manual", e.g. "manually".
query="attr(tags, '\bmanual\b', kind('${lintable_kinds}', //...))"
query+=" except attr(generator_function, '^docs', //...)"

mapfile -t candidates < <(bazel query "${query}")

if [ ${#candidates[@]} -eq 0 ]; then
    exit 0
fi

# cquery, not query, since compatibility is only known after configuration.
# Prints the label if compatible, else a blank line, which gets dropped below.
print_label_if_compatible='(
    "" if "IncompatiblePlatformProvider" in [str(type(p)) for p in providers(target).values()]
    else "//{}:{}".format(target.label.package, target.label.name)
)'

bazel cquery --config="${config}" "set(${candidates[*]})" \
    --output=starlark \
    --starlark:expr="${print_label_if_compatible}" \
    | sed '/^$/d'
