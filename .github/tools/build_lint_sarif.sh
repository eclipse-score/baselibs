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
# Cleans up one linter's raw rules_lint reports into
# /tmp/lint-sarif-clean/<tool>/ and reports whether that linter should fail
# the job. Merging every tool's cleaned reports into one SARIF file is done
# separately, once all linters have run.
#
# Usage: build_lint_sarif.sh <tool> <mnemonic>
#   tool     short name used in file/category names, e.g. "clang-tidy"
#   mnemonic aspect mnemonic used by rules_lint, e.g. "AspectRulesLintClangTidy"
#
# Exits non-zero, and prints a ::error:: annotation, if the linter reported a
# violation that should fail the job. clang-tidy has its own WarningsAsErrors
# config, so a plain warning is not enough there; every other linter fails the
# job on any finding. Requires clean_lint_sarif.py next to this script.
set -euo pipefail

tool="$1"
mnemonic="$2"
clean_dir="/tmp/lint-sarif-clean/${tool}"

mapfile -d '' -t raw_reports < <(find -L bazel-bin -name "*.${mnemonic}.report" -print0)

python3 "$(dirname "$0")/clean_lint_sarif.py" --tool "${tool}" --output-dir "${clean_dir}" "${raw_reports[@]}"

shopt -s nullglob
clean_reports=("${clean_dir}"/*.sarif)
if [ ${#clean_reports[@]} -eq 0 ]; then
    echo "::warning::no ${tool} SARIF results to report"
    mkdir -p "${clean_dir}"
    echo "{\"version\":\"2.1.0\",\"runs\":[{\"tool\":{\"driver\":{\"name\":\"${tool}\"}},\"results\":[]}]}" \
        >"${clean_dir}/empty.sarif"
fi

# Decide pass/fail independently of what made it into the SARIF above, so a
# violation dropped there (e.g. its location was outside the checked-out
# repo) can never go unnoticed.
violated=0
if [ "${tool}" = "clang-tidy" ]; then
    for report in "${raw_reports[@]}"; do
        if [ "$(cat "${report}.exit_code")" != "0" ]; then
            violated=1
            break
        fi
    done
elif [ ${#clean_reports[@]} -gt 0 ]; then
    violated=1
fi

if [ "${violated}" = "1" ]; then
    echo "::error::${tool} reported violations, see the SARIF/report artifacts and code scanning for details"
    exit 1
fi
