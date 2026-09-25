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
"""Unit tests for strip_optional_scalar_defaults.py."""

import unittest

from strip_optional_scalar_defaults import strip_null_defaults


class StripNullDefaultsTest(unittest.TestCase):
    def test_canonical_spelling(self):
        self.assertEqual(
            strip_null_defaults("table T {\n  a: uint32 = null;\n}\n"),
            "table T {\n  a: uint32;\n}\n",
        )

    def test_whitespace_variants(self):
        for field, expected in (
            ("a:uint32=null;", "a:uint32;"),
            ("a:uint32 =null;", "a:uint32;"),
            ("a:uint32\t=\tnull\t;", "a:uint32\t;"),
        ):
            with self.subTest(field=field):
                self.assertEqual(strip_null_defaults(field), expected)

    def test_default_split_across_lines(self):
        self.assertEqual(strip_null_defaults("a: uint32\n    = null;"), "a: uint32;")

    def test_other_defaults_are_kept(self):
        self.assertEqual(strip_null_defaults("a: uint32 = 0;"), "a: uint32 = 0;")

    def test_identifier_starting_with_null_is_kept(self):
        self.assertEqual(strip_null_defaults("a: E = nullable;"), "a: E = nullable;")

    def test_doc_comment_is_untouched(self):
        content = "/// Absent is written as a: uint32 = null; in the schema.\na: uint32 = null;\n"
        self.assertEqual(
            strip_null_defaults(content),
            "/// Absent is written as a: uint32 = null; in the schema.\na: uint32;\n",
        )

    def test_block_comment_is_untouched(self):
        content = (
            "/* a: uint32 = null;\n   still a comment = null; */\na: uint32 = null;"
        )
        self.assertEqual(
            strip_null_defaults(content),
            "/* a: uint32 = null;\n   still a comment = null; */\na: uint32;",
        )

    def test_default_followed_by_attribute_list(self):
        self.assertEqual(
            strip_null_defaults("a: uint32 = null (deprecated);"),
            "a: uint32 (deprecated);",
        )

    def test_string_literal_is_untouched(self):
        content = 'a: uint32 = null (attr: "x = null;");'
        self.assertEqual(strip_null_defaults(content), 'a: uint32 (attr: "x = null;");')

    def test_escaped_quote_in_string_literal(self):
        content = 'attribute "a\\" = null;";\nb: uint32 = null;'
        self.assertEqual(
            strip_null_defaults(content), 'attribute "a\\" = null;";\nb: uint32;'
        )

    def test_unterminated_comment_at_end_of_input(self):
        self.assertEqual(
            strip_null_defaults("a: uint32 = null;\n// tail"), "a: uint32;\n// tail"
        )

    def test_schema_without_defaults_is_unchanged(self):
        content = "namespace n;\n\ntable T {\n  a: string;\n}\n\nroot_type T;\n"
        self.assertEqual(strip_null_defaults(content), content)


if __name__ == "__main__":
    unittest.main()
