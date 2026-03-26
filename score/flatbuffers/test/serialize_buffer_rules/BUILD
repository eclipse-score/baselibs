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

load("@bazel_skylib//rules:build_test.bzl", "build_test")
load("//score/flatbuffers/bazel:tools.bzl", "serialize_buffer", "serialize_multiple_buffers")

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
    targets = [
        ":gen_multiple_data_bin",
        ":serialize_buffer_custom_name",
        ":serialize_buffer_default_name",
        ":serialize_buffer_output_in_subdir",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # build_test doesn't work under QEMU
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
    data = "subdir/data.json",
    output = "subdir/data_output_in_subdir.bin",
    schema = "not_existing_schema.fbs",
    tags = ["manual"],  # This target is expected to fail due to wrong schema path
)
