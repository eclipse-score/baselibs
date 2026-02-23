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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
        "spp_code_style_check_method_names",
        "spp_code_style_check_readability",
        "spp_code_style_check_type_names",
        "spp_code_style_check_variable_names",
    ],
)

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "signal_handling",
    srcs = [
        "signal_handling.cpp",
    ],
    hdrs = [
        "signal_handling.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/os/utils:signal",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "signal_handling_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/mw/log/detail:__pkg__"],
    deps = [
        ":signal_handling",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/os/utils/mocklib:signal_mock",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":unit_test",
    ],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
    ],
)
