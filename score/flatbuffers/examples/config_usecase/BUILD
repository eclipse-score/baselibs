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
load("@score_baselibs//score/flatbuffers/bazel:tools.bzl", "generate_json_schema", "serialize_buffer")

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

# Demo test: loads demo_config.bin at runtime, verifies and reads the config
# using filesystem and flatbufferscpp library.
cc_test(
    name = "demo_app",
    srcs = [
        "demo_app_test.cpp",
        ":demo_component_config",
    ],
    data = [":demo_config_bin"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/flatbuffers:flatbufferscpp",
        "@score_baselibs//score/flatbuffers:flatbufferutils",
    ],
)
