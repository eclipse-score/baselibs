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

load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

LIB_JSON_VISIBILITY = ["//visibility:public"]

string_flag(
    name = "base_library",
    build_setting_default = "vajson",
    values = [
        "vajson",
        "nlohmann",
    ],
)

config_setting(
    name = "base_library_vajson",
    flag_values = {":base_library": "vajson"},
    visibility = ["@score_baselibs//score/json/internal/parser:__subpackages__"],
)

config_setting(
    name = "base_library_nlohmann",
    flag_values = {":base_library": "nlohmann"},
    visibility = ["@score_baselibs//score/json/internal/parser:__subpackages__"],
)

cc_library(
    name = "parser_interface",
    srcs = ["i_json_parser.cpp"],
    hdrs = [
        "i_json_parser.h",
        "json_parser.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "writer_interface",
    srcs = ["i_json_writer.cpp"],
    hdrs = [
        "i_json_writer.h",
        "json_writer.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/filesystem/filestream",
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "interface",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":parser_interface",
        ":writer_interface",
    ],
)

cc_library(
    name = "json_parser_impl",
    srcs = ["json_parser.cpp"],
    features = COMPILER_WARNING_FEATURES,
    local_defines = select({
        ":base_library_nlohmann": ["NLOHMANN"],
        ":base_library_vajson": ["VAJSON"],
    }),
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        ":parser_interface",
        "@score_baselibs//score/json/internal/parser",
    ],
)

cc_library(
    name = "json_writer_impl",
    srcs = ["json_writer.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        ":writer_interface",
        "@score_baselibs//score/json/internal/writer/json_serialize",
        "@score_baselibs//score/language/safecpp/safe_math",
    ],
)

cc_library(
    name = "vajson_json_writer_impl",
    srcs = ["vajson_json_writer.cpp"],
    hdrs = ["vajson_json_writer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":writer_interface",
        "@score_baselibs//score/json/internal/parser/vajson/vajson_impl:vajson_serialize",
    ],
)

cc_library(
    name = "json",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FUSA"],
    visibility = LIB_JSON_VISIBILITY,
    deps = [
        ":json_parser_impl",
        ":json_writer_impl",
    ],
)

cc_library(
    name = "json_serializer",
    srcs = ["json_serializer.cpp"],
    hdrs = ["json_serializer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":json",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/result",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
    ],
)

alias(
    name = "json_parser",
    actual = ":json_parser_impl",
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

alias(
    name = "json_impl",
    actual = ":json",
    visibility = LIB_JSON_VISIBILITY,
)

cc_library(
    name = "mock",
    testonly = True,
    hdrs = [
        "i_json_parser_mock.h",
        "i_json_writer_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = LIB_JSON_VISIBILITY,
    deps = [
        ":interface",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "json_test",
    srcs = [
        "json_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
    ],
    deps = [
        ":json",
        "@googletest//:gtest_main",
    ],
)

# This rule was moved from json/internal/writer/BUILD file due to incorrect functionality of coverage tool.
# Was found a problem when unit_test_qnx didn't run tests from json_writer_test.cpp and json_serialize_test.cpp.
# Solution was move json_writer_test.cpp file to the same directory with json_writer.cpp.
# For json_serialize its BUILD file was modified according to other tests for json.
cc_test(
    name = "json_writer_unit_test",
    srcs = [
        "json_writer_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "test_warnings",
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/filesystem:mock",
        "@score_baselibs//score/json",
    ],
)

cc_test(
    name = "json_serializer_test",
    srcs = [
        "json_serializer_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        ":json_serializer",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":json_test",
        ":json_serializer_test",
        ":json_writer_unit_test",  # workaround to include coverage for json_writer
        "@score_baselibs//score/json/internal/model:unit_test",
        "@score_baselibs//score/json/internal/writer/json_serialize:json_serialize_unit_test",  # workaround to include coverage for json_serialize
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/json/internal/parser:unit_tests",
    ],
    visibility = [
        "//visibility:public",
    ],
)
