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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "json_builder",
    srcs = ["json_builder.cpp"],
    hdrs = ["json_builder.h"],
    tags = ["FFI"],
    deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/result",
        "@nlohmann_json//:json",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "nlohmann_parser",
    srcs = ["nlohmann_parser.cpp"],
    hdrs = ["nlohmann_parser.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        ":json_builder",
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/result",
        "@nlohmann_json//:json",
    ],
)

cc_test(
    name = "nlohmann_parser_test",
    srcs = ["nlohmann_parser_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["NLOHMANN"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/json/internal/parser:parser_test_suite",
        "@score_baselibs//score/json/internal/parser/nlohmann:nlohmann_parser",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "number_parser_test",
    srcs = ["number_parser_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["NLOHMANN"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        ":nlohmann_parser",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/json",
        "@score_baselibs//score/json/internal/parser:number_parser_test_suite",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":nlohmann_parser_test",
        ":number_parser_test",
    ],
    visibility = [
        "@score_baselibs//score/json/internal/parser:__pkg__",
    ],
)
