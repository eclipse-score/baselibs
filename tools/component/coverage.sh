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
#
# Coverage of ONE component, selected through its package metadata.
#
#   tools/component/coverage.sh //score/json:component
#
# 1. The component's tests are the test rules whose package_metadata names it.
# 2. `bazel coverage` runs exactly those tests (LLVM source-based coverage).
# 3. The existing reporter produces the HTML report and LCOV for the whole
#    coverage scope (other components show 0 % because their tests did not run).
# 4. The LCOV is joined with the component file list derived from the same
#    metadata, giving the component's own numbers.
#
# Run from the workspace root (not via `bazel run`, it drives bazel itself).
set -euo pipefail

component="${1:?usage: $0 //pkg:component_target [universe, default //score/...]}"
universe="${2:-//score/...}"
pkg="${component#//}"; pkg="${pkg%%:*}"
archive="coverage-report-${pkg//\//-}"
# Report only: the gate belongs to the full run, not to one component.
export COVERAGE_THRESHOLD="${COVERAGE_THRESHOLD:-0}"

echo "== tests of ${component}"
tests=$(bazel query "kind('.*_test rule', attr(package_metadata, '${component}', ${universe}))")
echo "${tests}"

echo "== bazel coverage (LLVM) on those tests"
# shellcheck disable=SC2086
bazel coverage --config=llvm_cov --build_tests_only -- ${tests}

echo "== reporter: HTML + LCOV for the scope"
bazel run @score_coverage//:generate_coverage_html -- \
  --yaml tools/coverage/coverage_justifications.yaml \
  --testlogs-subdir score \
  --archive-dir "${archive}"

echo "== files of the component (same metadata, current configuration)"
bazel build //tools/component:component_files
python3 tools/component/component_summary.py \
  --lcov "${archive}/coverage_report.dat" \
  --components bazel-bin/tools/component/component_files.json

echo
echo "HTML report: ${archive}/index.html"
