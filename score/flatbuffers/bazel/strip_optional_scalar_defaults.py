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
with the "= null" default rewritten away. The original .fbs (used for --cpp and --binary)
is untouched.

The rewrite only touches code: comments and string literals are skipped, so a "= null;"
inside a "///" doc comment (which flatc turns into a JSON-schema "description", from which
schema_generator.py parses metadata) or inside a quoted attribute value survives verbatim.

Usage:
    strip_optional_scalar_defaults.py --input <input.fbs> --output <output.fbs>
"""

import argparse
import re

# A scalar default of "null", with any (or no) whitespace around the "=". The trailing
# "\b" keeps identifiers such as "nullable" intact. What follows the default (";", or an
# attribute list like "(deprecated);") is left alone. Matched only outside comments and
# string literals.
_NULL_DEFAULT = re.compile(r"\s*=\s*null\b")

# The next lexical element that has to be skipped over: a line comment (which "///" doc
# comments are a special case of), a block comment, or a string literal.
_SKIPPED = re.compile(r'//|/\*|"')

# Where each skipped element ends. A string literal honours backslash escapes; an
# unterminated line comment ends at end of input.
_SKIPPED_END = {
    "//": re.compile(r"\n|\Z"),
    "/*": re.compile(r"\*/|\Z"),
    '"': re.compile(r'(?:[^"\\\n]|\\.)*(?:"|\Z)'),
}


def strip_null_defaults(content):
    """Remove every "= null" scalar default from the code parts of a .fbs schema.

    Effectively it's a small hand-rolled lexer/tokenizer. It never applies the
    "remove null default" regex inside comments or strings, only in actual schema code,
    avoiding false-positive matches like a "= null" appearing in a doc comment.
    """
    out = []
    pos = 0
    while True:
        skipped = _SKIPPED.search(content, pos)
        code_end = skipped.start() if skipped else len(content)
        out.append(_NULL_DEFAULT.sub("", content[pos:code_end]))
        if not skipped:
            return "".join(out)
        terminator = _SKIPPED_END[skipped.group()].search(content, skipped.end())
        end = terminator.end() if terminator else len(content)
        out.append(content[skipped.start() : end])
        pos = end


def main():
    parser = argparse.ArgumentParser(
        description='Strip optional-scalar "= null;" defaults from a .fbs schema.'
    )
    parser.add_argument("--input", required=True, help="Input .fbs schema file path")
    parser.add_argument("--output", required=True, help="Output patched .fbs file path")

    args = parser.parse_args()

    with open(args.input, "r") as f:
        content = f.read()

    content = strip_null_defaults(content)

    with open(args.output, "w") as f:
        f.write(content)


if __name__ == "__main__":
    main()
