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

alias(
    name = "parser",
    actual = select({
        "@score_baselibs//score/json:base_library_nlohmann": "@score_baselibs//score/json/internal/parser/nlohmann:nlohmann_parser",
        "@score_baselibs//score/json:base_library_vajson": "@score_baselibs//score/json/internal/parser/vajson:vajson_parser",
    }),
    visibility = ["@score_baselibs//score/json:__pkg__"],
)

cc_library(
    name = "parser_test_suite",
    testonly = True,
    srcs = ["parsers_test_suite.cpp"],
    hdrs = ["parsers_test_suite.h"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/json",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "json_parser_test",
    srcs = ["json_parser_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/json",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "number_parser_test_suite",
    testonly = True,
    srcs = ["number_parser_test_suite.cpp"],
    hdrs = ["number_parser_test_suite.h"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/json",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":json_parser_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/json/internal/parser/nlohmann:unit_test_suite",
        "@score_baselibs//score/json/internal/parser/vajson:unit_test_suite",
    ],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
    ],
)
