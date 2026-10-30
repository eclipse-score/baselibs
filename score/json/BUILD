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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_docs_as_code//:docs.bzl", "docs_bundle")

LIB_JSON_VISIBILITY = ["//visibility:public"]

# No-op, kept only so consumers who still pass --base_library on the command line or in their
# .bazelrc don't get a hard "no such target" build failure. vaJson is the only parser backend;
# this flag no longer selects anything. Remove in the next breaking release.
string_flag(
    name = "base_library",
    build_setting_default = "vajson",
    deprecation = "base_library no longer has any effect, vaJson is the only parser backend. " +
                  "Remove this flag from your build configuration; it will be deleted in a future release.",
    values = [
        "vajson",
        "nlohmann",
    ],
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
    name = "formatting",
    hdrs = ["formatting.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
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
        ":formatting",
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
        # Coverage scope root (//tools/coverage, eclipse-score/baselibs#512).
        "//tools/coverage:__pkg__",
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
    implementation_deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/json/internal/parser/vajson",
        "@score_baselibs//score/result",
    ],
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        ":parser_interface",
    ],
)

cc_library(
    name = "json_writer_impl",
    srcs = ["json_writer.cpp"],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "@score_baselibs//score/json/internal/writer:serializer",
    ],
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        ":writer_interface",
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

cc_test(
    name = "json_writer_test",
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

docs_bundle(
    name = "docs",
    code_targets = [
        ":json",
        ":json_serializer",
    ],
    primary_need_id = "comp__baselibs_json",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)
