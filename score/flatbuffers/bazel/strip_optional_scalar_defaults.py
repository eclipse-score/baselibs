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

"""Strips optional-scalar "= null;" defaults from a .fbs schema for --jsonschema.

flatc --jsonschema does not support optional scalars ("field: T = null;"), while --cpp
and --binary do. Optionality has no JSON-schema representation anyway (an omitted key is
already "absent" in JSON), so this script feeds generate_json_schema a copy of the schema
with " = null;" mechanically rewritten to ";". The original .fbs (used for --cpp and
--binary) is untouched.

Usage:
    strip_optional_scalar_defaults.py --input <input.fbs> --output <output.fbs>
"""

import argparse


def main():
    parser = argparse.ArgumentParser(
        description='Strip optional-scalar "= null;" defaults from a .fbs schema.'
    )
    parser.add_argument("--input", required=True, help="Input .fbs schema file path")
    parser.add_argument("--output", required=True, help="Output patched .fbs file path")

    args = parser.parse_args()

    with open(args.input, "r") as f:
        content = f.read()

    content = content.replace(" = null;", ";")

    with open(args.output, "w") as f:
        f.write(content)


if __name__ == "__main__":
    main()
