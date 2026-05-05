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
load("//score/flatbuffers/bazel:tools.bzl", "serialize_buffer", "serialize_multiple_buffers", "serialize_multiple_versioned_buffers", "serialize_versioned_buffer")

serialize_buffer(
    name = "serialize_buffer_default_name",
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "valid_data.bin",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

serialize_buffer(
    name = "serialize_buffer_custom_name",
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "my_custom_data.bin",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

serialize_buffer(
    name = "serialize_buffer_output_in_subdir",
    data = "//score/flatbuffers/test/testdata:valid_data3.json",
    output = "subdir/data_output_in_subdir.bin",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

serialize_multiple_buffers(
    name = "gen_multiple_data_bin",
    data_dict = {
        "//score/flatbuffers/test/testdata:valid_data.json": "multiple_data.bin",
        "//score/flatbuffers/test/testdata:valid_data2.json": "multiple_data2.bin",
        "//score/flatbuffers/test/testdata:valid_data3.json": "subdir/multiple_data_in_subdir.bin",
    },
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

build_test(
    name = "serialize_buffer_test",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # build_test doesn't work under QEMU
    targets = [
        ":gen_multiple_data_bin",
        ":serialize_buffer_custom_name",
        ":serialize_buffer_default_name",
        ":serialize_buffer_output_in_subdir",
    ],
)

# ===================================================
# Versioned buffer rule tests
# ===================================================

serialize_versioned_buffer(
    name = "serialize_versioned_buffer_single",
    buffer_major_version = 1,
    buffer_minor_version = 2,
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "versioned_data.bin",
    schema = "//score/flatbuffers/test/testdata:test_versioned.fbs",
)

serialize_multiple_versioned_buffers(
    name = "serialize_versioned_buffer_multiple",
    buffer_major_version = 3,
    buffer_minor_version = 0,
    data_dict = {
        "//score/flatbuffers/test/testdata:valid_data.json": "versioned_multi_1.bin",
        "//score/flatbuffers/test/testdata:valid_data2.json": "versioned_multi_2.bin",
    },
    schema = "//score/flatbuffers/test/testdata:test_versioned.fbs",
)

build_test(
    name = "serialize_versioned_buffer_test",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # build_test doesn't work under QEMU
    targets = [
        ":serialize_versioned_buffer_single",
        ":serialize_versioned_buffer_multiple",
    ],
)

# ===================================================
# Manual: Rule fault injection tests for development.
# ===================================================

# Invalid config - should produce error during build
# This demonstrates error handling for malformed JSON
serialize_buffer(
    name = "serialize_buffer_invalid_json",
    data = "//score/flatbuffers/test/testdata:invalid_data.json",
    output = "invalid_data.bin",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
    tags = ["manual"],
)

serialize_buffer(
    name = "serialize_wrong_schema_path",
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "subdir/data_output_in_subdir.bin",
    schema = "not_existing_schema.fbs",
    tags = ["manual"],  # This target is expected to fail due to wrong schema path
)

# Demonstrates that serialize_versioned_buffer fails at build time when a version
# value is outside of uint16_t range [0, 65535]
serialize_versioned_buffer(
    name = "serialize_versioned_buffer_negative_version",
    buffer_major_version = -1,
    buffer_minor_version = 0,
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "versioned_negative_version.bin",
    schema = "//score/flatbuffers/test/testdata:test_versioned.fbs",
    tags = ["manual"],  # Expected to fail: -1 is outside of uint16_t range [0, 65535]
)

serialize_versioned_buffer(
    name = "serialize_versioned_buffer_overflow_version",
    buffer_major_version = 65536,
    buffer_minor_version = 0,
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "versioned_overflow_version.bin",
    schema = "//score/flatbuffers/test/testdata:test_versioned.fbs",
    tags = ["manual"],  # Expected to fail: 65536 is outside of uint16_t range [0, 65535]
)

# Demonstrates that serialize_versioned_buffer fails at build time when version_info
# is placed in a nested subtable instead of the root table.
# The inject step adds `version_info` to the root of the JSON; --strict-json then
# rejects it because MyComponentConfig in this schema has no such field.
serialize_versioned_buffer(
    name = "serialize_versioned_buffer_wrong_placement",
    buffer_major_version = 1,
    buffer_minor_version = 0,
    data = "//score/flatbuffers/test/testdata:valid_data.json",
    output = "versioned_wrong_placement.bin",
    schema = "//score/flatbuffers/test/testdata:test_versioned_wrong_placement.fbs",
    tags = ["manual"],  # Expected to fail: injected version_info rejected by --strict-json
)
