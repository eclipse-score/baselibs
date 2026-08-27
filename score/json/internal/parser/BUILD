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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
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
        "@googletest//:gtest",
        "@score_baselibs//score/json",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
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
        "@googletest//:gtest_main",
        "@score_baselibs//score/json",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
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
        "@googletest//:gtest_main",
        "@score_baselibs//score/json",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
    ],
)
