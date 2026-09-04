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

"""Cleans up rules_lint's per-file clang-tidy SARIF reports for GitHub code scanning.

rules_lint's clang-tidy aspect emits one SARIF file per linted source file, with
no `ruleId` on its results, and locations reported as `%SRCROOT%`-relative URIs.
None of this is directly usable for a GitHub code scanning upload:

* GitHub rejects a SARIF upload if any result is missing a `ruleId` that
  resolves against that run's `tool.driver.rules`.
* GitHub ignores `uriBaseId`/`originalUriBaseIds` entirely and resolves
  `artifactLocation.uri` as a path relative to the repository root, so a
  leading "./" must be stripped.

This script fixes up each report in place: it synthesizes `ruleId` from the
check name clang-tidy already appends to each message (for example
"... [modernize-use-trailing-return-type]"), and normalizes paths. Combining
the cleaned-up reports into a single run and enforcing GitHub's size limits is
left to `sarif-multitool merge` and `sarif-multitool rewrite --normalize-for-ghas`.
"""

import argparse
import json
import os
import re
import sys

RULE_ID_RE = re.compile(r"\[([\w.,-]+)\]$")


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
    # rules_lint reports paths relative to %SRCROOT%, e.g. "./score/result/error.h".
    # GitHub resolves artifactLocation.uri directly against the repository root
    # and does not honor uriBaseId, so the leading "./" must go.
    return uri[2:] if uri.startswith("./") else uri


def rule_id_for(result):
    # clang-tidy appends every check that fired as a comma-separated list, e.g.
    # "... [cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers]"
    # when a diagnostic matches more than one aliased check. A SARIF result can
    # only carry a single ruleId, so use the first name in that list.
    message = result.get("message", {}).get("text", "")
    match = RULE_ID_RE.search(message)
    return match.group(1).split(",")[0] if match else "clang-tidy"


def result_files(result):
    for location in result.get("locations", []):
        artifact = location.get("physicalLocation", {}).get("artifactLocation", {})
        if "uri" in artifact:
            yield normalize_uri(artifact["uri"])


def clean_report(report):
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

            # clang-tidy also lints headers pulled in from external repos and
            # toolchains (e.g. "external/...llvm_toolchain/..."). GitHub can
            # only resolve paths that exist in the checked-out repository, so
            # drop anything else.
            if not all(os.path.isfile(f) for f in result_files(result)):
                dropped_outside_repo += 1
                continue

            result["ruleId"] = rule_id_for(result)
            kept.append(result)
        run["results"] = kept

    return sum(
        len(run.get("results", [])) for run in report.get("runs", [])
    ), dropped_outside_repo


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--output-dir",
        required=True,
        help="directory to write the cleaned SARIF files to",
    )
    parser.add_argument(
        "reports", nargs="*", help="clang-tidy SARIF report files to clean up"
    )
    args = parser.parse_args()

    os.makedirs(args.output_dir, exist_ok=True)

    total_results = 0
    total_dropped_outside_repo = 0
    written = 0

    for path in args.reports:
        report = load_report(path)
        if report is None:
            continue

        result_count, dropped_outside_repo = clean_report(report)
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
