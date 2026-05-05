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

"""Patches a JSON data file with buffer version information.

This script is invoked by the serialize_buffer Bazel rule to inject
major_version and minor_version into the user's JSON data file before
flatc serialization. The version is stored as a 'version' object at
the top level of the JSON.

Usage:
    inject_buffer_version.py --input <input.json> --output <output.json> \
                     --major <major_version> --minor <minor_version>
"""

import argparse
import json


def main():
    parser = argparse.ArgumentParser(
        description="Patch a JSON data file with buffer version information."
    )
    parser.add_argument("--input", required=True, help="Input JSON file path")
    parser.add_argument("--output", required=True, help="Output patched JSON file path")
    parser.add_argument(
        "--major", type=int, required=True, help="Major version number (0-65535)"
    )
    parser.add_argument(
        "--minor", type=int, required=True, help="Minor version number (0-65535)"
    )

    args = parser.parse_args()

    _UINT16_MAX = 65535
    if not (0 <= args.major <= _UINT16_MAX):
        parser.error(
            f"--major {args.major} is out of uint16_t range [0, {_UINT16_MAX}]"
        )
    if not (0 <= args.minor <= _UINT16_MAX):
        parser.error(
            f"--minor {args.minor} is out of uint16_t range [0, {_UINT16_MAX}]"
        )

    with open(args.input, "r") as f:
        data = json.load(f)

    data["version_info"] = {
        "major_version": args.major,
        "minor_version": args.minor,
    }

    with open(args.output, "w") as f:
        json.dump(data, f, indent=2)
        f.write("\n")


if __name__ == "__main__":
    main()
