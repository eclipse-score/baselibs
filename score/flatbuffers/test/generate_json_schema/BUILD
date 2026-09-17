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

load("@bazel_skylib//rules:build_test.bzl", "build_test")
load("@bazel_skylib//rules:diff_test.bzl", "diff_test")
load("//score/flatbuffers/bazel:tools.bzl", "generate_json_schema")

generate_json_schema(
    name = "gen_json_schema_default_name",
    output = "test.schema.json",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

generate_json_schema(
    name = "gen_json_schema_custom_name",
    output = "test_my_custom_name.schema.json",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

generate_json_schema(
    name = "gen_json_schema_in_subdir",
    output = "subdir/test_output_in_subdir.schema.json",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

build_test(
    name = "gen_json_schema_test",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # build_test doesn't work under QEMU
    targets = [
        ":gen_json_schema_custom_name",
        ":gen_json_schema_default_name",
        ":gen_json_schema_in_subdir",
    ],
)

# End-to-end check of the whole flatc --jsonschema -> schema_generator.py pipeline against a
# real .fbs fixture that uses every supported comment token (@title, @default, @min, @max,
# @required, @shared) plus (required) fields, a deprecated field, an enum, a union, vectors,
# a fixed-size array and a reference cycle. Unlike the unit tests -- which feed hand-written
# raw dictionaries to the Python -- this fails if flatc changes the shape it emits for
# descriptions, $refs, required lists or array bounds.
generate_json_schema(
    name = "gen_rich_json_schema",
    output = "test_rich_schema.schema.json",
    schema = "//score/flatbuffers/test/testdata:test_rich_schema.fbs",
)

# Regenerate the golden after an intentional change with:
#   bazel build --config=bl-x86_64-linux \
#     //score/flatbuffers/test/generate_json_schema:gen_rich_json_schema && \
#     cp -f bazel-bin/score/flatbuffers/test/generate_json_schema/test_rich_schema.schema.json \
#       score/flatbuffers/test/generate_json_schema/test_rich_schema.golden.schema.json
diff_test(
    name = "gen_rich_json_schema_diff_test",
    file1 = "test_rich_schema.golden.schema.json",
    file2 = ":gen_rich_json_schema",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # flatc is only built for the exec platform
)

# ===================================================
# Manual: Rule fault injection tests for development.
# ===================================================

generate_json_schema(
    name = "gen_json_schema_wrong_schema_path",
    output = "not_existing_schema.schema.json",
    schema = "not_existing_schema.fbs",
    tags = ["manual"],  # This target is expected to fail due to wrong schema path
)
