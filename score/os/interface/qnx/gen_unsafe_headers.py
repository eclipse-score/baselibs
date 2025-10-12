# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
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
"""Prints all relative paths in the QNX SDP at `target/qnx7/usr/include` that are not contained in SAFE_HEADERS.
"""

import argparse
import os
from pathlib import Path


INCLUDE_PATH = Path("target/qnx7/usr/include")
SAFE_HEADERS_PATH = Path("platform/aas/lib/os/interface/qnx/safe_headers.txt")


def parse_safe_headers(path):
    safe_headers = set()
    with open(path, "r") as file:
        for line in file.readlines():
            line = line.strip()
            if len(line) > 0:
                safe_headers.add(line)
    return safe_headers


def find_unsafe_headers(include_path, safe_headers):
    paths = set()

    for root, _, files in os.walk(include_path):
        for file in files:
            dir_path = os.path.relpath(root, include_path)
            path = os.path.relpath(os.path.join(dir_path, file))

            if path not in safe_headers:
                paths.add(path)

    return paths


def main():
    parser = argparse.ArgumentParser(description="Generate list of unsafe QNX headers")
    parser.add_argument(
        "--qnx_root", help="path to the QNX SDP root directory", type=Path
    )

    args = parser.parse_args()

    include_path = args.qnx_root.expanduser() / INCLUDE_PATH

    safe_headers_path = os.getcwd() / SAFE_HEADERS_PATH
    safe_headers = parse_safe_headers(safe_headers_path)

    # Find C headers (exclude C++ headers)
    paths = find_unsafe_headers(include_path, safe_headers)
    unsafe_c_headers = {path for path in paths if "c++" not in path}

    # Find LLVM libc++ headers (GNU libstdc++ headers at path c++/8.3.0 are QM)
    unsafe_cxx_headers = find_unsafe_headers(include_path / "c++/v1", safe_headers)

    unsafe_headers = list(unsafe_c_headers | unsafe_cxx_headers)
    unsafe_headers.sort()

    for unsafe_header in unsafe_headers:
        print(unsafe_header)


if __name__ == "__main__":
    main()
