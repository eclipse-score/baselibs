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

load("//score/flatbuffers/bazel:codegen.bzl", "generate_cpp")
load("//score/flatbuffers/bazel:tools.bzl", "serialize_multiple_versioned_buffers", "serialize_versioned_buffer")

exports_files(
    [
        "test.fbs",
        "test_versioned.fbs",
        "test_versioned_min.fbs",
        "test_versioned_max.fbs",
        "test_versioned_min_range.fbs",
        "test_versioned_max_range.fbs",
        "test_versioned_wrong_placement.fbs",
        "test_versioned_wrong_field.fbs",
        "invalid_data.json",
        "valid_data.json",
        "valid_data2.json",
        "valid_data3.json",
        "versioned_buffer_data.json",
    ],
    visibility = ["//score/flatbuffers/test:__subpackages__"],
)

# Serialize the versioned buffer data
serialize_versioned_buffer(
    name = "gen_versioned_buffer_bin",
    buffer_major_version = 2,
    buffer_minor_version = 3,
    data = "versioned_buffer_data.json",
    output = "versioned_buffer.bin",
    schema = "test_versioned.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Serialize the versioned buffer data with explicit ids in schema
serialize_versioned_buffer(
    name = "gen_versioned_buffer_explicit_ids_bin",
    buffer_major_version = 2,
    buffer_minor_version = 3,
    data = "versioned_buffer_data.json",
    output = "versioned_buffer_explicit_ids.bin",
    schema = "test_versioned_explicit_ids.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Wrong placement of version_info can only be detected by VersionReader
serialize_versioned_buffer(
    name = "gen_versioned_buffer_version_info_wrong_field",
    buffer_major_version = 1,
    buffer_minor_version = 2,
    data = "versioned_buffer_data.json",
    output = "versioned_wrong_field.bin",
    schema = "test_versioned_wrong_field.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Wrong placement of version_info can only be detected by VersionReader
serialize_versioned_buffer(
    name = "gen_versioned_buffer_version_info_wrong_field_similar_type",
    buffer_major_version = 1,
    buffer_minor_version = 2,
    data = "versioned_buffer_data_wrong_field_similar_type.json",
    output = "versioned_wrong_field_similar_type.bin",
    schema = "test_versioned_wrong_field_similar_type.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Minimum version buffers
# Tolerate the minimum version (0.0) for the versioned buffer data
# Identifier: '    '
serialize_versioned_buffer(
    name = "gen_versioned_min_buffer_bin",
    buffer_major_version = 0,
    buffer_minor_version = 0,
    data = "versioned_buffer_data.json",
    output = "versioned_min_buffer.bin",
    schema = "test_versioned_min.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Identifier: ' !"#'
serialize_versioned_buffer(
    name = "gen_versioned_min_range_buffer_bin",
    buffer_major_version = 0,
    buffer_minor_version = 1,
    data = "versioned_buffer_data.json",
    output = "versioned_min_range_buffer.bin",
    schema = "test_versioned_min_range.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Maximum version buffers
# Identifier: '{|}~'
serialize_versioned_buffer(
    name = "gen_versioned_max_range_buffer_bin",
    buffer_major_version = 65535,
    buffer_minor_version = 65534,
    data = "versioned_buffer_data.json",
    output = "versioned_max_range_buffer.bin",
    schema = "test_versioned_max_range.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)

# Identifier: '~~~~'
serialize_versioned_buffer(
    name = "gen_versioned_max_buffer_bin",
    buffer_major_version = 65535,
    buffer_minor_version = 65535,
    data = "versioned_buffer_data.json",
    output = "versioned_max_buffer.bin",
    schema = "test_versioned_max.fbs",
    visibility = ["//score/flatbuffers:__pkg__"],
)
