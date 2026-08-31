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
"""Post-process flatc --jsonschema output into a rich JSON Schema.

This tool reads the raw JSON schema output from flatc --jsonschema and post-processes it
into a rich JSON schema in draft-2020-12 format with extracted metadata.

The Bazel rule handles flatc invocation; this script only does post-processing.

Input: Raw JSON schema file from ``flatc --jsonschema`` (draft 2019-09 format with definitions + $ref)
Output: Rich JSON schema in draft-2020-12 format

Post-processing steps:
  * split ``@title:`` / ``@default:`` / ``@min:`` / ``@max:`` / ``@required`` token lines
    out of each ``description`` into proper JSON-schema attributes;
  * strip flatc's type-based ``minimum`` / ``maximum``, re-adding only from ``@min`` / ``@max``;
  * inline every ``$ref`` except tables marked ``@shared: <name>`` in the .fbs, which
    are lifted into ``$defs/<name>`` and referenced (used for tables shared by 2+ parents);
  * additionally lift any table on a reference cycle into ``$defs`` under its flatc name,
    since inlining a cycle would never terminate.

The result is deterministic (fixed key ordering, ``json.dumps(indent=4)``), so the checked-in
schema is simply this tool's committed output and a drift test can compare byte-for-byte.
"""

import argparse
import json
import re
import sys

_TOKEN_RE = re.compile(r"^@(title|default|min|max|required|shared)\b\s*:?\s*(.*)$")

_DRAFT_2020_12 = "https://json-schema.org/draft/2020-12/schema"


class GenerationError(RuntimeError):
    """Raised when schema generation fails."""


def _parse_default(raw):
    """Parse an ``@default:`` token value into its JSON type (bool / int / string)."""
    if raw == "true":
        return True
    if raw == "false":
        return False
    try:
        return int(raw)
    except ValueError:
        return raw


def _parse_description(text):
    """Split a flatc ``description`` string into (metadata, clean_description).

    ``@token`` lines are extracted into a dict; the remaining lines form the human-readable
    description (joined with ``\\n``, preserving the original multi-line layout).
    """
    meta = {
        "title": None,
        "default": None,
        "min": None,
        "max": None,
        "required": False,
        "shared": None,
    }
    desc_lines = []
    for line in text.split("\n"):
        match = _TOKEN_RE.match(line)
        if match is None:
            desc_lines.append(line)
            continue
        key, value = match.group(1), match.group(2).strip()
        if key == "required":
            meta["required"] = True
        elif key in ("min", "max"):
            meta[key] = int(value)
        elif key == "default":
            meta["default"] = _parse_default(value)
        elif key == "shared":
            meta["shared"] = value
        else:  # title
            meta["title"] = value
    return meta, "\n".join(desc_lines)


class _Enricher:
    def __init__(self, definitions, shared_defs):
        self._defs = definitions
        # Maps a full (namespaced) def name -> the ``$defs`` key it is lifted into.
        self._shared_defs = shared_defs

    def _ref_name(self, node):
        return node["$ref"].split("/")[-1]

    def _is_enum(self, def_name):
        return "enum" in self._defs[def_name]

    def _enum_values(self, def_name):
        return self._defs[def_name]["enum"]

    def build_field(self, node):
        """Build an enriched schema node for a property. Returns (schema_node, required)."""
        if "$ref" in node:
            ref = self._ref_name(node)
            if ref in self._shared_defs:
                meta, _ = _parse_description(node.get("description", ""))
                return {"$ref": "#/$defs/%s" % self._shared_defs[ref]}, meta["required"]
            if self._is_enum(ref):
                meta, desc = _parse_description(node.get("description", ""))
                out = {"type": "string"}
                if meta["title"] is not None:
                    out["title"] = meta["title"]
                if desc:
                    out["description"] = desc
                out["enum"] = self._enum_values(ref)
                if meta["default"] is not None:
                    out["default"] = meta["default"]
                return out, meta["required"]
            # Non-lifted table reference -> inline it. Such fields are authored bare
            # (metadata lives on the referenced table), so they are never required here.
            return self.build_object(self._defs[ref]), False

        node_type = node.get("type")
        if node_type == "array":
            meta, desc = _parse_description(node.get("description", ""))
            out = {"type": "array"}
            if meta["title"] is not None:
                out["title"] = meta["title"]
            if desc:
                out["description"] = desc
            out["items"] = self._build_items(node["items"])
            # Fixed-length FlatBuffer arrays: flatc pins the length via minItems/maxItems.
            if "minItems" in node:
                out["minItems"] = node["minItems"]
            if "maxItems" in node:
                out["maxItems"] = node["maxItems"]
            return out, meta["required"]
        # union type (anyOf) is used for union fields, which are authored bare
        # metadata lives on the referenced table, so they are never required here.
        if "anyOf" in node:
            meta, desc = _parse_description(node.get("description", ""))
            out = {}
            if meta["title"] is not None:
                out["title"] = meta["title"]
            if desc:
                out["description"] = desc
            out["anyOf"] = [self._build_items(alt) for alt in node["anyOf"]]
            return out, meta["required"]

        # Scalar / string / bool leaf.
        meta, desc = _parse_description(node.get("description", ""))
        out = {"type": node_type}
        if meta["title"] is not None:
            out["title"] = meta["title"]
        if desc:
            out["description"] = desc
        if "enum" in node:  # inline enum defined directly on the node (not via $ref)
            out["enum"] = node["enum"]
        if meta["default"] is not None:
            out["default"] = meta["default"]
        if meta["min"] is not None:
            out["minimum"] = meta["min"]
        if meta["max"] is not None:
            out["maximum"] = meta["max"]
        if node.get("deprecated"):
            out["deprecated"] = True
        return out, meta["required"]

    def _build_items(self, items):
        if "$ref" in items:
            ref = self._ref_name(items)
            if ref in self._shared_defs:
                return {"$ref": "#/$defs/%s" % self._shared_defs[ref]}
            if self._is_enum(ref):
                return {"type": "string", "enum": self._enum_values(ref)}
            return self.build_object(self._defs[ref])
        # Scalar vector items: drop flatc's type-based min/max (schema has none for these).
        return {"type": items["type"]}

    def build_object(self, def_node):
        meta, desc = _parse_description(def_node.get("description", ""))
        properties = {}
        required = []
        flatc_required = set(def_node.get("required", []))
        for field_name, field_node in def_node.get("properties", {}).items():
            built, is_required = self.build_field(field_node)
            properties[field_name] = built
            if is_required or field_name in flatc_required:
                required.append(field_name)
        out = {"type": "object"}
        if meta["title"] is not None:
            out["title"] = meta["title"]
        if desc:
            out["description"] = desc
        if required:
            out["required"] = required
        out["additionalProperties"] = False
        out["properties"] = properties
        return out


def _collect_shared_defs(definitions):
    """Map full def name -> ``$defs`` key for every table marked ``@shared: <name>``.

    Iteration follows ``definitions`` order (i.e. .fbs order), keeping ``$defs`` deterministic.
    """
    shared = {}
    for full_name, def_node in definitions.items():
        meta, _ = _parse_description(def_node.get("description", ""))
        name = meta["shared"]
        if name is None:
            continue
        if name in shared.values():
            raise GenerationError("duplicate @shared name: %s" % name)
        shared[full_name] = name
    return shared


def _iter_ref_names(node):
    """Yield the name of every definition referenced by a property node."""
    if "$ref" in node:
        yield node["$ref"].split("/")[-1]
    items = node.get("items")
    if isinstance(items, dict):
        yield from _iter_ref_names(items)
    for alt in node.get("anyOf", []):
        yield from _iter_ref_names(alt)


def _collect_cycle_defs(definitions, shared):
    """Map full def name -> ``$defs`` key for every table that must be lifted to break a cycle.

    Inlining a table reference recurses into the referenced table, so a reference cycle --
    which FlatBuffers permits, since it stores tables by offset -- would never terminate.
    Lifting one table per cycle into ``$defs`` breaks it: a lifted table is emitted as a
    ``$ref`` instead of being inlined. Tables already marked ``@shared`` break cycles for
    the same reason, so they are skipped here and treated as cuts.

    Iteration follows ``definitions`` order (i.e. .fbs order), so the chosen cut -- and
    therefore the output -- is deterministic. The full flatc name is used as the ``$defs``
    key: unlike ``@shared`` names it is not author-chosen, and it is unique by construction.
    """
    edges = {
        name: list(
            dict.fromkeys(
                ref
                for field in node.get("properties", {}).values()
                for ref in _iter_ref_names(field)
            )
        )
        for name, node in definitions.items()
    }

    def reaches_itself(start, cuts):
        """Whether ``start`` is reachable from itself without passing through a cut."""
        stack, seen = [start], set()
        while stack:
            for nxt in edges.get(stack.pop(), ()):
                if nxt == start:
                    return True
                if nxt not in cuts and nxt not in seen:
                    seen.add(nxt)
                    stack.append(nxt)
        return False

    cycle = {}
    for full_name in definitions:
        if full_name in shared:
            continue
        # Every table lifted so far is emitted as a ``$ref``, cutting paths through it.
        if reaches_itself(full_name, set(shared) | set(cycle)):
            cycle[full_name] = full_name
    for name in cycle.values():
        if name in shared.values():
            raise GenerationError("@shared name collides with cycle def: %s" % name)
    return cycle


def generate(raw_schema_json):
    """Post-process raw flatc JSON schema into rich draft-2020-12 format.

    Args:
        raw_schema_json: Raw JSON schema dict from flatc --jsonschema (draft 2019-09 format)

    Returns:
        Rich JSON schema as string in draft-2020-12 format
    """
    raw = raw_schema_json
    definitions = raw["definitions"]
    shared_defs = _collect_shared_defs(definitions)
    # Tables on a reference cycle are lifted alongside the ``@shared`` ones: both are
    # emitted as ``$ref``, which is what stops the inlining from recursing forever.
    shared_defs.update(_collect_cycle_defs(definitions, shared_defs))
    enricher = _Enricher(definitions, shared_defs)

    root_name = raw["$ref"].split("/")[-1]
    root_obj = enricher.build_object(definitions[root_name])

    schema = {"$schema": _DRAFT_2020_12}
    if "title" in root_obj:
        schema["title"] = root_obj["title"]
    if "description" in root_obj:
        schema["description"] = root_obj["description"]
    schema["type"] = "object"
    if "required" in root_obj:
        schema["required"] = root_obj["required"]
    schema["additionalProperties"] = root_obj["additionalProperties"]
    schema["properties"] = root_obj["properties"]
    schema["$defs"] = {
        name: enricher.build_object(definitions[full])
        for full, name in shared_defs.items()
    }

    return json.dumps(schema, indent=4, ensure_ascii=False) + "\n"


def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--raw-schema",
        required=True,
        help="Path to the raw JSON schema file from flatc --jsonschema.",
    )
    parser.add_argument(
        "--output",
        default="-",
        help="Where to write the schema, or '-' for stdout (default: stdout).",
    )
    args = parser.parse_args(argv)

    # Read the raw schema from flatc
    with open(args.raw_schema, "r", encoding="utf-8") as handle:
        raw_schema_json = json.load(handle)

    schema = generate(raw_schema_json)
    if args.output == "-":
        sys.stdout.write(schema)
    else:
        with open(args.output, "w", encoding="utf-8") as handle:
            handle.write(schema)
    return 0


if __name__ == "__main__":
    sys.exit(main())
