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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

package(default_visibility = ["@score_baselibs//score/json:__subpackages__"])

cc_library(
    name = "vajson",
    srcs = [
        "json_data.cpp",
        "json_ops.cpp",
        "structure_parser.cpp",
    ],
    hdrs = [
        "depth_counter.h",
        "json_data.h",
        "json_error_domain.h",
        "json_number.h",
        "json_ops.h",
        "number_parser.h",
        "opt_char.h",
        "parser_state.h",
        "reader_config.h",
        "reader_fwd.h",
        "structure_parser.h",
        "types.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/string_view:zstring_view",
        "@score_baselibs//score/result",
    ],
)

cc_gtest_unit_test(
    name = "unit_test",
    srcs = [
        "depth_counter_test.cpp",
        "json_data_test.cpp",
        "json_error_domain_test.cpp",
        "json_number_test.cpp",
        "json_ops_test.cpp",
        "number_parser_test.cpp",
        "opt_char_test.cpp",
        "structure_parser_array_test.cpp",
        "structure_parser_bool_test.cpp",
        "structure_parser_invalid_test.cpp",
        "structure_parser_key_test.cpp",
        "structure_parser_null_test.cpp",
        "structure_parser_number_test.cpp",
        "structure_parser_object_test.cpp",
        "structure_parser_string_test.cpp",
        "structure_parser_test.cpp",
        "structure_parser_valid_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":structure_parser_stub",
        ":vajson",
    ],
)

cc_library(
    name = "structure_parser_stub",
    testonly = True,
    hdrs = ["structure_parser_stub.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:private"],
    deps = [":vajson"],
)
