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
load("//score/flatbuffers/bazel:codegen.bzl", "generate_cpp")

generate_cpp(
    name = "gen_cpp_default_flatc_name",
    output = "test_generated.h",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

generate_cpp(
    name = "gen_cpp_custom_name",
    output = "test_my_custom_name.h",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

generate_cpp(
    name = "gen_cpp_in_subdir",
    output = "subdir/test_output_in_subdir.h",
    schema = "//score/flatbuffers/test/testdata:test.fbs",
)

build_test(
    name = "gen_cpp_build_test",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],  # build_test doesn't work under QEMU
    targets = [
        ":gen_cpp_default_flatc_name",
        ":gen_cpp_custom_name",
        ":gen_cpp_in_subdir",
    ],
)

# ===================================================
# Manual: Rule fault injection tests for development.
# ===================================================

generate_cpp(
    name = "gen_cpp_wrong_schema_path",
    output = "not_existing_schema.h",
    schema = "not_existing_schema.fbs",
    tags = ["manual"],  # This target is expected to fail due to wrong schema path
)
