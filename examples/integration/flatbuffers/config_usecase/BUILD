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

load("@rules_cc//cc:defs.bzl", "cc_test")
load("@score_baselibs//score/flatbuffers/bazel:codegen.bzl", "generate_cpp")
load("@score_baselibs//score/flatbuffers/bazel:tools.bzl", "generate_json_schema", "serialize_buffer", "serialize_versioned_buffer")

# Generate C++ header from FlatBuffer schema
generate_cpp(
    name = "demo_component_config",
    output = "component_config.h",
    schema = "demo.fbs",
)

# Generate JSON schema from FlatBuffer schema
generate_json_schema(
    name = "demo_schema",
    output = "demo.schema.json",
    schema = "demo.fbs",
)

# Generate binary config from JSON config and FlatBuffer schema
serialize_buffer(
    name = "demo_config_bin",
    data = "demo_config.json",
    output = "demo_config.bin",
    schema = "demo.fbs",
)

# Demo test: loads demo_config.bin at runtime, verifies and reads the config.
cc_test(
    name = "demo_config",
    size = "small",
    srcs = [
        "demo_config_test.cpp",
        ":demo_component_config",
    ],
    data = [":demo_config_bin"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/flatbuffers:flatbufferutils",
    ],
)

# Generate C++ header from the versioned FlatBuffer schema, which includes
# buffer_version.fbs and defines version_info at id: 0 in the root table.
generate_cpp(
    name = "demo_component_config_versioned",
    output = "component_config_versioned.h",
    schema = "demo_versioned.fbs",
)

# Generate binary config with injected version (major=1, minor=0) from the
# same JSON data as the unversioned demo.
serialize_versioned_buffer(
    name = "demo_config_versioned_bin",
    buffer_major_version = 1,
    buffer_minor_version = 0,
    data = "demo_config.json",
    output = "demo_config_versioned.bin",
    schema = "demo_versioned.fbs",
)

# Demo versioned test: loads demo_config_versioned.bin, calls VerifyVersion
# before accessing the data, then reads and asserts config values.
cc_test(
    name = "demo_config_versioned",
    size = "small",
    srcs = [
        "demo_config_versioned_test.cpp",
        ":demo_component_config_versioned",
    ],
    data = [":demo_config_versioned_bin"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/flatbuffers:flatbufferutils",
    ],
)
