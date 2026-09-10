#!/usr/bin/env python3

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

"""Cleans up rules_lint's per-target SARIF reports for GitHub code scanning.

clang-tidy's SARIF comes from a generic errorformat-based converter with no
`ruleId` set, and paths reported as `%SRCROOT%`-relative URIs. Clippy's SARIF
comes from a dedicated converter that already sets `ruleId` from rustc's own
diagnostics.

Neither is directly usable for a code scanning upload as-is:

* GitHub rejects a SARIF upload if any result is missing a `ruleId` that
  resolves against that run's `tool.driver.rules`.
* GitHub ignores `uriBaseId`/`originalUriBaseIds` and resolves
  `artifactLocation.uri` relative to the repository root, so a leading
  "./" must be stripped.

This script fixes up each report in place: synthesizes `ruleId` where
missing, normalizes paths, and drops results that point outside the
checked-out repository (vendored deps, toolchain headers). Merging the
cleaned-up reports and enforcing GitHub's size limits is left to
`sarif-multitool merge` and `sarif-multitool rewrite --normalize-for-ghas`.
"""

import argparse
import json
import os
import re
import sys

# clang-tidy appends every check that fired as a comma-separated list, e.g.
# "... [cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers]".
# A SARIF result can only carry one ruleId, so use the first name.
CLANG_TIDY_RULE_ID_RE = re.compile(r"\[([\w.,-]+)\]$")

RULE_ID_EXTRACTORS = {
    "clang-tidy": lambda message: (
        CLANG_TIDY_RULE_ID_RE.search(message).group(1).split(",")[0]
        if CLANG_TIDY_RULE_ID_RE.search(message)
        else "clang-tidy"
    ),
    # Clippy's ruleId (e.g. "clippy::needless_return") is already set.
    "clippy": None,
}


def load_report(path):
    with open(path, encoding="utf-8") as f:
        try:
            return json.load(f)
        except json.JSONDecodeError as e:
            print(
                f"warning: skipping unparseable SARIF file {path}: {e}", file=sys.stderr
            )
            return None


def normalize_uri(uri):
    # GitHub resolves artifactLocation.uri against the repository root and
    # ignores uriBaseId, so the "./" rules_lint uses must go.
    return uri[2:] if uri.startswith("./") else uri


def result_files(result):
    for location in result.get("locations", []):
        artifact = location.get("physicalLocation", {}).get("artifactLocation", {})
        if "uri" in artifact:
            yield normalize_uri(artifact["uri"])


def clean_report(report, rule_id_extractor):
    dropped_outside_repo = 0

    for run in report.get("runs", []):
        kept = []
        for result in run.get("results", []):
            for location in result.get("locations", []):
                artifact = location.get("physicalLocation", {}).get(
                    "artifactLocation", {}
                )
                if "uri" in artifact:
                    artifact["uri"] = normalize_uri(artifact["uri"])
                artifact.pop("uriBaseId", None)

            # Drop results in files outside the checked-out repo (external
            # deps, toolchains); GitHub can't resolve those paths anyway.
            if not all(os.path.isfile(f) for f in result_files(result)):
                dropped_outside_repo += 1
                continue

            if rule_id_extractor and not result.get("ruleId"):
                result["ruleId"] = rule_id_extractor(
                    result.get("message", {}).get("text", "")
                )
            kept.append(result)
        run["results"] = kept

    return sum(
        len(run.get("results", [])) for run in report.get("runs", [])
    ), dropped_outside_repo


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--tool",
        required=True,
        choices=sorted(RULE_ID_EXTRACTORS),
        help="which linter produced the reports, to select the ruleId synthesis strategy",
    )
    parser.add_argument(
        "--output-dir",
        required=True,
        help="directory to write the cleaned SARIF files to",
    )
    parser.add_argument(
        "reports", nargs="*", help="rules_lint SARIF report files to clean up"
    )
    args = parser.parse_args()

    os.makedirs(args.output_dir, exist_ok=True)
    rule_id_extractor = RULE_ID_EXTRACTORS[args.tool]

    total_results = 0
    total_dropped_outside_repo = 0
    written = 0

    for path in args.reports:
        report = load_report(path)
        if report is None:
            continue

        result_count, dropped_outside_repo = clean_report(report, rule_id_extractor)
        total_results += result_count
        total_dropped_outside_repo += dropped_outside_repo

        if result_count == 0:
            continue

        out_name = path.replace(os.sep, "__") + ".sarif"
        with open(os.path.join(args.output_dir, out_name), "w", encoding="utf-8") as f:
            json.dump(report, f)
        written += 1

    if total_dropped_outside_repo:
        print(
            f"dropped {total_dropped_outside_repo} result(s) outside the checked-out repository",
            file=sys.stderr,
        )
    print(
        f"cleaned {len(args.reports)} report(s) into {written} file(s) with {total_results} result(s)",
        file=sys.stderr,
    )


if __name__ == "__main__":
    main()
