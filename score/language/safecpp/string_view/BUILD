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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "null_termination_violation_policies",
    srcs = ["null_termination_violation_policies.cpp"],
    hdrs = ["null_termination_violation_policies.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

cc_gtest_unit_test(
    name = "null_termination_violation_policies_test",
    srcs = [
        "null_termination_violation_policies_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "throws_upon_exception",
    ],
    deps = [
        ":null_termination_violation_policies",
    ],
)

cc_library(
    name = "null_termination_check",
    srcs = ["null_termination_check.cpp"],
    hdrs = ["null_termination_check.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":null_termination_violation_policies",
        ":zspan",
        ":zstring_view",
    ],
)

cc_gtest_unit_test(
    name = "null_termination_check_test",
    srcs = [
        "null_termination_check_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "throws_upon_exception",
    ],
    deps = [
        ":null_termination_check",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "zspan",
    srcs = ["zspan.cpp"],
    hdrs = ["zspan.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":null_termination_violation_policies",
    ],
)

cc_gtest_unit_test(
    name = "zspan_test",
    srcs = [
        "zspan_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "throws_upon_exception",
    ],
    deps = [
        ":zspan",
        ":zstring_view",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "zstring_view",
    srcs = ["zstring_view.cpp"],
    hdrs = ["zstring_view.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":zspan",
    ],
)

cc_gtest_unit_test(
    name = "zstring_view_test",
    srcs = [
        "zstring_view_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":zstring_view",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":zspan_test",
        ":zstring_view_test",
        ":null_termination_check_test",
        ":null_termination_violation_policies_test",
    ],
    visibility = ["@score_baselibs//score/language/safecpp:__pkg__"],
)
