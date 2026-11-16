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
"""Line and branch coverage per component from an LCOV file and a
component file list (the JSON written by score_component_files).

Demo only: the same join is what docs-as-code's lcov_summary.py does with its
docs_bundle file lists, and what the coverage reporter would do with the
metadata directly.
"""

import argparse
import json
import sys
from collections import defaultdict


def parse_lcov(path):
    """{source path: {"lines": {no: hits}, "branches": {(line, block, br): hits}}}"""
    files = {}
    cur = None
    with open(path, encoding="utf-8") as fh:
        for raw in fh:
            line = raw.strip()
            if line.startswith("SF:"):
                cur = files.setdefault(line[3:], {"lines": {}, "branches": {}})
            elif cur is None:
                continue
            elif line.startswith("DA:"):
                no, hits = line[3:].split(",")[:2]
                cur["lines"][int(no)] = cur["lines"].get(int(no), 0) + int(hits)
            elif line.startswith("BRDA:"):
                ln, block, br, hits = line[5:].split(",")
                key = (int(ln), block, br)
                hits = 0 if hits == "-" else int(hits)
                cur["branches"][key] = cur["branches"].get(key, 0) + hits
            elif line == "end_of_record":
                cur = None
    return files


def normalise(path):
    for prefix in ("./", "external/"):
        if path.startswith(prefix) and prefix == "./":
            path = path[len(prefix):]
    return path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lcov", required=True)
    ap.add_argument("--components", required=True, help="JSON from score_component_files")
    ap.add_argument("--only", help="Restrict to one component id")
    args = ap.parse_args()

    lcov = {normalise(k): v for k, v in parse_lcov(args.lcov).items()}
    comps = json.load(open(args.components, encoding="utf-8"))["components"]

    print(f"{'component':32s} {'files':>9s} {'lines':>12s} {'branches':>12s}")
    for comp in comps:
        cid = comp["id"]
        if args.only and cid != args.only:
            continue
        lines_total = lines_hit = br_total = br_hit = measured = 0
        for f in comp["files"]:
            rec = lcov.get(f)
            if rec is None:
                continue
            measured += 1
            lines_total += len(rec["lines"])
            lines_hit += sum(1 for h in rec["lines"].values() if h > 0)
            br_total += len(rec["branches"])
            br_hit += sum(1 for h in rec["branches"].values() if h > 0)

        def pct(hit, total):
            return "n/a" if total == 0 else f"{round(100 * hit / total):3d}% ({hit}/{total})"

        print(f"{cid:32s} {measured:4d}/{len(comp['files']):<4d} {pct(lines_hit, lines_total):>12s} {pct(br_hit, br_total):>12s}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
