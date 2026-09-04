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
"""Unit tests for schema_generator.py."""

import io
import json
import os
import tempfile
import unittest
from contextlib import redirect_stdout

import schema_generator as sg


def _table(description=None, properties=None, required=None):
    """Build a flatc-style table definition node."""
    node = {"type": "object", "additionalProperties": False}
    if description is not None:
        node["description"] = description
    if required is not None:
        node["required"] = required
    node["properties"] = properties or {}
    return node


def _ref(name, description=None):
    node = {"$ref": "#/definitions/%s" % name}
    if description is not None:
        node["description"] = description
    return node


class ParseDefaultTest(unittest.TestCase):
    def test_booleans(self):
        self.assertIs(sg._parse_default("true"), True)
        self.assertIs(sg._parse_default("false"), False)

    def test_integers(self):
        self.assertEqual(sg._parse_default("42"), 42)
        self.assertEqual(sg._parse_default("-7"), -7)

    def test_floats(self):
        self.assertEqual(sg._parse_default("1.5"), 1.5)
        self.assertEqual(sg._parse_default("-0.25"), -0.25)
        self.assertEqual(sg._parse_default("1e-3"), 0.001)
        self.assertIsInstance(sg._parse_default("1.5"), float)

    def test_non_number_stays_string(self):
        self.assertEqual(sg._parse_default("hello world"), "hello world")
        self.assertEqual(sg._parse_default(""), "")
        # Not JSON number literals, so they stay verbatim.
        self.assertEqual(sg._parse_default("1.5f"), "1.5f")
        self.assertEqual(sg._parse_default("nan"), "nan")
        self.assertEqual(sg._parse_default("0x10"), "0x10")

    def test_capitalized_booleans_are_strings(self):
        self.assertEqual(sg._parse_default("True"), "True")


class ParseDescriptionTest(unittest.TestCase):
    def test_empty(self):
        meta, desc = sg._parse_description("")
        self.assertEqual(desc, "")
        self.assertEqual(
            meta,
            {
                "title": None,
                "default": None,
                "min": None,
                "max": None,
                "required": False,
                "shared": None,
            },
        )

    def test_all_tokens(self):
        meta, desc = sg._parse_description(
            "@title: My Title\n"
            "@default: 3\n"
            "@min: 1\n"
            "@max: 10\n"
            "@required\n"
            "@shared: MyShared\n"
            "human text"
        )
        self.assertEqual(meta["title"], "My Title")
        self.assertEqual(meta["default"], 3)
        self.assertEqual(meta["min"], 1)
        self.assertEqual(meta["max"], 10)
        self.assertTrue(meta["required"])
        self.assertEqual(meta["shared"], "MyShared")
        self.assertEqual(desc, "human text")

    def test_multi_line_description_layout_is_preserved(self):
        meta, desc = sg._parse_description("first\n@title: T\nsecond\n\nthird")
        self.assertEqual(meta["title"], "T")
        self.assertEqual(desc, "first\nsecond\n\nthird")

    def test_tokens_accept_missing_colon_and_extra_space(self):
        meta, _ = sg._parse_description("@title   Spaced\n@min   5")
        self.assertEqual(meta["title"], "Spaced")
        self.assertEqual(meta["min"], 5)

    def test_negative_min_max(self):
        meta, _ = sg._parse_description("@min: -3\n@max: -1")
        self.assertEqual(meta["min"], -3)
        self.assertEqual(meta["max"], -1)

    def test_fractional_min_max(self):
        meta, _ = sg._parse_description("@min: 0.5\n@max: 1e3")
        self.assertEqual(meta["min"], 0.5)
        self.assertEqual(meta["max"], 1000.0)

    def test_non_numeric_min_is_an_error(self):
        with self.assertRaises(sg.GenerationError) as ctx:
            sg._parse_description("@min: lots")
        self.assertIn("@min", str(ctx.exception))

    def test_unknown_token_is_plain_description(self):
        meta, desc = sg._parse_description("@unknown: x")
        self.assertIsNone(meta["title"])
        self.assertEqual(desc, "@unknown: x")

    def test_token_must_start_the_line(self):
        _, desc = sg._parse_description("see @title: not a token")
        self.assertEqual(desc, "see @title: not a token")

    def test_last_token_wins(self):
        meta, _ = sg._parse_description("@title: one\n@title: two")
        self.assertEqual(meta["title"], "two")


class BuildFieldTest(unittest.TestCase):
    def _enricher(self, definitions=None, shared=None):
        return sg._Enricher(definitions or {}, shared or {})

    def test_scalar_leaf_full_metadata(self):
        enricher = self._enricher()
        out, required = enricher.build_field(
            {
                "type": "integer",
                "minimum": -128,  # flatc's type-based bound: must be dropped
                "maximum": 127,
                "description": "@title: Count\n@default: 3\n@min: 1\n@max: 10\n@required\nhow many",
            }
        )
        self.assertTrue(required)
        self.assertEqual(
            out,
            {
                "type": "integer",
                "title": "Count",
                "description": "how many",
                "default": 3,
                "minimum": 1,
                "maximum": 10,
            },
        )

    def test_scalar_leaf_drops_flatc_bounds_when_no_tokens(self):
        out, required = self._enricher().build_field(
            {"type": "integer", "minimum": 0, "maximum": 255}
        )
        self.assertEqual(out, {"type": "integer"})
        self.assertFalse(required)

    def test_scalar_key_order_is_deterministic(self):
        out, _ = self._enricher().build_field(
            {
                "type": "integer",
                "description": "@title: T\n@max: 9\n@min: 1\n@default: 2\nd",
            }
        )
        self.assertEqual(
            list(out), ["type", "title", "description", "default", "minimum", "maximum"]
        )

    def test_inline_enum_on_node(self):
        out, _ = self._enricher().build_field(
            {"type": "string", "enum": ["a", "b"], "description": "@default: a"}
        )
        self.assertEqual(out, {"type": "string", "enum": ["a", "b"], "default": "a"})

    def test_deprecated_flag(self):
        out, _ = self._enricher().build_field({"type": "boolean", "deprecated": True})
        self.assertEqual(out, {"type": "boolean", "deprecated": True})

    def test_deprecated_false_is_omitted(self):
        out, _ = self._enricher().build_field({"type": "boolean", "deprecated": False})
        self.assertEqual(out, {"type": "boolean"})

    def test_default_false_is_emitted(self):
        # ``False`` is not ``None``, so an explicit ``@default: false`` survives.
        out, _ = self._enricher().build_field(
            {"type": "boolean", "description": "@default: false"}
        )
        self.assertEqual(out, {"type": "boolean", "default": False})

    def test_default_zero_is_emitted(self):
        out, _ = self._enricher().build_field(
            {"type": "integer", "description": "@default: 0"}
        )
        self.assertEqual(out, {"type": "integer", "default": 0})

    def test_min_zero_is_emitted(self):
        out, _ = self._enricher().build_field(
            {"type": "integer", "description": "@min: 0\n@max: 0"}
        )
        self.assertEqual(out, {"type": "integer", "minimum": 0, "maximum": 0})

    def test_ref_to_enum(self):
        enricher = self._enricher({"NS.Color": {"enum": ["RED", "GREEN"]}})
        out, required = enricher.build_field(
            _ref("NS.Color", "@title: Color\n@default: RED\n@required\npick one")
        )
        self.assertTrue(required)
        self.assertEqual(
            out,
            {
                "type": "string",
                "title": "Color",
                "description": "pick one",
                "enum": ["RED", "GREEN"],
                "default": "RED",
            },
        )

    def test_ref_to_shared_def(self):
        enricher = self._enricher({"NS.Thing": _table()}, shared={"NS.Thing": "Thing"})
        out, required = enricher.build_field(_ref("NS.Thing", "@required"))
        self.assertEqual(out, {"$ref": "#/$defs/Thing"})
        self.assertTrue(required)

    def test_ref_to_shared_def_keeps_field_annotations(self):
        # Draft 2020-12 allows annotation siblings next to a ``$ref``.
        enricher = self._enricher({"NS.Thing": _table()}, shared={"NS.Thing": "Thing"})
        node = _ref("NS.Thing", "@title: The Thing\nwhat it does")
        node["deprecated"] = True
        out, _ = enricher.build_field(node)
        self.assertEqual(
            out,
            {
                "$ref": "#/$defs/Thing",
                "title": "The Thing",
                "description": "what it does",
                "deprecated": True,
            },
        )

    def test_ref_to_plain_table_is_inlined_and_never_required(self):
        enricher = self._enricher(
            {"NS.Thing": _table("@title: Thing", {"a": {"type": "integer"}})}
        )
        out, required = enricher.build_field(_ref("NS.Thing", "@required"))
        self.assertFalse(required)
        self.assertEqual(
            out,
            {
                "type": "object",
                "title": "Thing",
                "additionalProperties": False,
                "properties": {"a": {"type": "integer"}},
            },
        )

    def test_inlined_table_field_annotations_win_over_the_table(self):
        enricher = self._enricher(
            {"NS.Thing": _table("@title: Thing\ntable doc", {"a": {"type": "integer"}})}
        )
        node = _ref("NS.Thing", "@title: Field\nfield doc")
        node["deprecated"] = True
        out, _ = enricher.build_field(node)
        self.assertEqual(
            out,
            {
                "type": "object",
                "title": "Field",
                "description": "field doc",
                "additionalProperties": False,
                "properties": {"a": {"type": "integer"}},
                "deprecated": True,
            },
        )

    def test_inlined_table_keeps_its_own_annotations_when_field_is_bare(self):
        enricher = self._enricher({"NS.Thing": _table("@title: Thing\ntable doc")})
        out, _ = enricher.build_field(_ref("NS.Thing"))
        self.assertEqual(out["title"], "Thing")
        self.assertEqual(out["description"], "table doc")

    def test_deprecated_ref_to_enum(self):
        enricher = self._enricher({"NS.Color": {"enum": ["RED"]}})
        node = _ref("NS.Color", "@title: Color")
        node["deprecated"] = True
        out, _ = enricher.build_field(node)
        self.assertEqual(
            out,
            {
                "type": "string",
                "title": "Color",
                "enum": ["RED"],
                "deprecated": True,
            },
        )

    def test_deprecated_array(self):
        out, _ = self._enricher().build_field(
            {"type": "array", "items": {"type": "integer"}, "deprecated": True}
        )
        self.assertEqual(
            out, {"type": "array", "items": {"type": "integer"}, "deprecated": True}
        )

    def test_deprecated_union(self):
        enricher = self._enricher({"NS.A": _table()})
        out, _ = enricher.build_field(
            {"anyOf": [_ref("NS.A")], "description": "@title: U", "deprecated": True}
        )
        self.assertEqual(list(out), ["title", "anyOf", "deprecated"])

    def test_array_of_scalars_drops_item_bounds(self):
        out, required = self._enricher().build_field(
            {
                "type": "array",
                "description": "@title: List\n@required\nitems here",
                "items": {"type": "integer", "minimum": 0, "maximum": 255},
            }
        )
        self.assertTrue(required)
        self.assertEqual(
            out,
            {
                "type": "array",
                "title": "List",
                "description": "items here",
                "items": {"type": "integer"},
            },
        )

    def test_fixed_length_array_keeps_min_max_items(self):
        out, _ = self._enricher().build_field(
            {
                "type": "array",
                "items": {"type": "integer"},
                "minItems": 4,
                "maxItems": 4,
            }
        )
        self.assertEqual(out["minItems"], 4)
        self.assertEqual(out["maxItems"], 4)

    def test_array_of_enum_refs(self):
        enricher = self._enricher({"NS.Color": {"enum": ["RED"]}})
        out, _ = enricher.build_field({"type": "array", "items": _ref("NS.Color")})
        self.assertEqual(out["items"], {"type": "string", "enum": ["RED"]})

    def test_array_of_shared_refs(self):
        enricher = self._enricher({"NS.Thing": _table()}, shared={"NS.Thing": "Thing"})
        out, _ = enricher.build_field({"type": "array", "items": _ref("NS.Thing")})
        self.assertEqual(out["items"], {"$ref": "#/$defs/Thing"})

    def test_array_of_inlined_tables(self):
        enricher = self._enricher(
            {"NS.Thing": _table(properties={"a": {"type": "string"}})}
        )
        out, _ = enricher.build_field({"type": "array", "items": _ref("NS.Thing")})
        self.assertEqual(
            out["items"],
            {
                "type": "object",
                "additionalProperties": False,
                "properties": {"a": {"type": "string"}},
            },
        )

    def test_union_any_of(self):
        enricher = self._enricher(
            {
                "NS.A": _table(properties={"a": {"type": "string"}}),
                "NS.B": _table(),
            },
            shared={"NS.B": "B"},
        )
        out, required = enricher.build_field(
            {
                "anyOf": [_ref("NS.A"), _ref("NS.B")],
                "description": "@title: Either\none of two",
            }
        )
        self.assertFalse(required)
        self.assertEqual(out["title"], "Either")
        self.assertEqual(out["description"], "one of two")
        self.assertEqual(out["anyOf"][1], {"$ref": "#/$defs/B"})
        self.assertEqual(out["anyOf"][0]["type"], "object")


class BuildObjectTest(unittest.TestCase):
    def test_required_from_tokens_and_flatc(self):
        enricher = sg._Enricher({}, {})
        out = enricher.build_object(
            _table(
                description="@title: Root\nroot doc",
                properties={
                    "a": {"type": "string", "description": "@required"},
                    "b": {"type": "string"},
                    "c": {"type": "string"},
                },
                required=["c"],
            )
        )
        self.assertEqual(out["title"], "Root")
        self.assertEqual(out["description"], "root doc")
        self.assertEqual(out["required"], ["a", "c"])
        self.assertFalse(out["additionalProperties"])
        self.assertEqual(list(out["properties"]), ["a", "b", "c"])

    def test_no_required_key_when_nothing_required(self):
        out = sg._Enricher({}, {}).build_object(
            _table(properties={"a": {"type": "string"}})
        )
        self.assertNotIn("required", out)
        self.assertEqual(list(out), ["type", "additionalProperties", "properties"])

    def test_empty_table(self):
        self.assertEqual(
            sg._Enricher({}, {}).build_object({"type": "object"}),
            {"type": "object", "additionalProperties": False, "properties": {}},
        )

    def test_shared_token_is_stripped_from_description(self):
        out = sg._Enricher({}, {}).build_object(_table(description="@shared: X\ndoc"))
        self.assertEqual(out["description"], "doc")


class CollectSharedDefsTest(unittest.TestCase):
    def test_collects_in_definition_order(self):
        definitions = {
            "NS.A": _table("@shared: Alpha"),
            "NS.B": _table("plain"),
            "NS.C": _table("@shared: Gamma"),
        }
        self.assertEqual(
            sg._collect_shared_defs(definitions), {"NS.A": "Alpha", "NS.C": "Gamma"}
        )

    def test_none_shared(self):
        self.assertEqual(sg._collect_shared_defs({"NS.A": _table()}), {})

    def test_duplicate_shared_name_raises(self):
        definitions = {
            "NS.A": _table("@shared: Dup"),
            "NS.B": _table("@shared: Dup"),
        }
        with self.assertRaises(sg.GenerationError) as ctx:
            sg._collect_shared_defs(definitions)
        self.assertIn("Dup", str(ctx.exception))


class IterRefNamesTest(unittest.TestCase):
    def test_direct_ref(self):
        self.assertEqual(list(sg._iter_ref_names(_ref("NS.A"))), ["NS.A"])

    def test_array_items(self):
        self.assertEqual(
            list(sg._iter_ref_names({"type": "array", "items": _ref("NS.A")})), ["NS.A"]
        )

    def test_any_of(self):
        self.assertEqual(
            list(sg._iter_ref_names({"anyOf": [_ref("NS.A"), _ref("NS.B")]})),
            ["NS.A", "NS.B"],
        )

    def test_scalar_items_yield_nothing(self):
        self.assertEqual(
            list(sg._iter_ref_names({"type": "array", "items": {"type": "integer"}})),
            [],
        )

    def test_plain_scalar(self):
        self.assertEqual(list(sg._iter_ref_names({"type": "string"})), [])


class CollectCycleDefsTest(unittest.TestCase):
    def test_no_cycle(self):
        definitions = {
            "NS.Root": _table(properties={"a": _ref("NS.A")}),
            "NS.A": _table(),
        }
        self.assertEqual(sg._collect_cycle_defs(definitions, {}), {})

    def test_self_reference(self):
        definitions = {"NS.A": _table(properties={"me": _ref("NS.A")})}
        self.assertEqual(sg._collect_cycle_defs(definitions, {}), {"NS.A": "NS.A"})

    def test_mutual_cycle_cuts_only_the_first_table(self):
        definitions = {
            "NS.A": _table(properties={"b": _ref("NS.B")}),
            "NS.B": _table(properties={"a": _ref("NS.A")}),
        }
        self.assertEqual(sg._collect_cycle_defs(definitions, {}), {"NS.A": "NS.A"})

    def test_cut_choice_follows_definition_order(self):
        definitions = {
            "NS.B": _table(properties={"a": _ref("NS.A")}),
            "NS.A": _table(properties={"b": _ref("NS.B")}),
        }
        self.assertEqual(sg._collect_cycle_defs(definitions, {}), {"NS.B": "NS.B"})

    def test_cycle_through_array_and_union(self):
        definitions = {
            "NS.A": _table(properties={"bs": {"type": "array", "items": _ref("NS.B")}}),
            "NS.B": _table(properties={"a": {"anyOf": [_ref("NS.A")]}}),
        }
        self.assertEqual(sg._collect_cycle_defs(definitions, {}), {"NS.A": "NS.A"})

    def test_shared_table_already_breaks_the_cycle(self):
        definitions = {
            "NS.A": _table("@shared: Alpha", {"b": _ref("NS.B")}),
            "NS.B": _table(properties={"a": _ref("NS.A")}),
        }
        self.assertEqual(sg._collect_cycle_defs(definitions, {"NS.A": "Alpha"}), {})

    def test_two_independent_cycles_are_both_cut(self):
        definitions = {
            "NS.A": _table(properties={"me": _ref("NS.A")}),
            "NS.B": _table(properties={"me": _ref("NS.B")}),
        }
        self.assertEqual(
            sg._collect_cycle_defs(definitions, {}), {"NS.A": "NS.A", "NS.B": "NS.B"}
        )

    def test_shared_name_colliding_with_cycle_def_raises(self):
        definitions = {
            "Foo": _table("@shared: Bar"),
            "Bar": _table(properties={"me": _ref("Bar")}),
        }
        with self.assertRaises(sg.GenerationError) as ctx:
            sg._collect_cycle_defs(definitions, {"Foo": "Bar"})
        self.assertIn("Bar", str(ctx.exception))


class GenerateTest(unittest.TestCase):
    def test_full_schema(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {
                "NS.Color": {"type": "string", "enum": ["RED", "GREEN"]},
                "NS.Shared": _table(
                    "@shared: Shared\n@title: Shared Thing\nreused",
                    {"n": {"type": "integer", "description": "@min: 1"}},
                ),
                "NS.Inline": _table(properties={"s": {"type": "string"}}),
                "NS.Root": _table(
                    "@title: Root\nthe root",
                    {
                        "color": _ref("NS.Color", "@required"),
                        "shared": _ref("NS.Shared"),
                        "inline": _ref("NS.Inline"),
                        "names": {"type": "array", "items": {"type": "string"}},
                    },
                ),
            },
        }
        schema = json.loads(sg.generate(raw))
        self.assertEqual(schema["$schema"], sg._DRAFT_2020_12)
        self.assertEqual(schema["title"], "Root")
        self.assertEqual(schema["description"], "the root")
        self.assertEqual(schema["type"], "object")
        self.assertEqual(schema["required"], ["color"])
        self.assertFalse(schema["additionalProperties"])
        self.assertEqual(
            schema["properties"]["color"],
            {"type": "string", "enum": ["RED", "GREEN"]},
        )
        self.assertEqual(schema["properties"]["shared"], {"$ref": "#/$defs/Shared"})
        self.assertEqual(schema["properties"]["inline"]["type"], "object")
        self.assertEqual(
            schema["$defs"]["Shared"],
            {
                "type": "object",
                "title": "Shared Thing",
                "description": "reused",
                "additionalProperties": False,
                "properties": {"n": {"type": "integer", "minimum": 1}},
            },
        )

    def test_top_level_key_order(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {
                "NS.Root": _table(
                    "@title: Root\ndoc",
                    {"a": {"type": "string", "description": "@required"}},
                )
            },
        }
        self.assertEqual(
            list(json.loads(sg.generate(raw))),
            [
                "$schema",
                "title",
                "description",
                "type",
                "required",
                "additionalProperties",
                "properties",
                "$defs",
            ],
        )

    def test_root_without_title_description_or_required(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {"NS.Root": _table(properties={"a": {"type": "string"}})},
        }
        schema = json.loads(sg.generate(raw))
        self.assertNotIn("title", schema)
        self.assertNotIn("description", schema)
        self.assertNotIn("required", schema)
        self.assertEqual(schema["$defs"], {})

    def test_cycle_is_lifted_into_defs(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {
                "NS.Node": _table(
                    properties={
                        "child": _ref("NS.Node"),
                        "name": {"type": "string"},
                    }
                ),
                "NS.Root": _table(properties={"node": _ref("NS.Node")}),
            },
        }
        schema = json.loads(sg.generate(raw))
        self.assertEqual(schema["properties"]["node"], {"$ref": "#/$defs/NS.Node"})
        self.assertEqual(
            schema["$defs"]["NS.Node"]["properties"]["child"],
            {"$ref": "#/$defs/NS.Node"},
        )

    def test_output_is_deterministic_and_ends_with_newline(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {"NS.Root": _table(properties={"a": {"type": "string"}})},
        }
        first = sg.generate(raw)
        self.assertEqual(first, sg.generate(raw))
        self.assertTrue(first.endswith("\n"))
        self.assertIn('\n    "type": "object"', first)  # indent=4

    def test_non_ascii_is_not_escaped(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {"NS.Root": _table("@title: Grüße")},
        }
        self.assertIn("Grüße", sg.generate(raw))

    def test_duplicate_shared_name_propagates(self):
        raw = {
            "$ref": "#/definitions/NS.Root",
            "definitions": {
                "NS.A": _table("@shared: Dup"),
                "NS.B": _table("@shared: Dup"),
                "NS.Root": _table(),
            },
        }
        with self.assertRaises(sg.GenerationError):
            sg.generate(raw)


class MainTest(unittest.TestCase):
    RAW = {
        "$ref": "#/definitions/NS.Root",
        "definitions": {"NS.Root": _table("@title: Root", {"a": {"type": "string"}})},
    }

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.raw_path = os.path.join(self._tmp.name, "raw.json")
        with open(self.raw_path, "w", encoding="utf-8") as handle:
            json.dump(self.RAW, handle)

    def test_writes_to_stdout_by_default(self):
        buffer = io.StringIO()
        with redirect_stdout(buffer):
            exit_code = sg.main(["--raw-schema", self.raw_path])
        self.assertEqual(exit_code, 0)
        self.assertEqual(buffer.getvalue(), sg.generate(self.RAW))

    def test_writes_to_output_file(self):
        out_path = os.path.join(self._tmp.name, "out.json")
        self.assertEqual(
            sg.main(["--raw-schema", self.raw_path, "--output", out_path]), 0
        )
        with open(out_path, "r", encoding="utf-8") as handle:
            self.assertEqual(handle.read(), sg.generate(self.RAW))

    def test_explicit_dash_output_is_stdout(self):
        buffer = io.StringIO()
        with redirect_stdout(buffer):
            sg.main(["--raw-schema", self.raw_path, "--output", "-"])
        self.assertEqual(buffer.getvalue(), sg.generate(self.RAW))

    def test_missing_required_argument_exits(self):
        with self.assertRaises(SystemExit) as ctx:
            with redirect_stdout(io.StringIO()):
                sg.main([])
        self.assertNotEqual(ctx.exception.code, 0)


if __name__ == "__main__":
    unittest.main()
