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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

cc_library(
    name = "scoped_operation",
    srcs = ["src/scoped_operation.cpp"],
    hdrs = ["src/scoped_operation.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "base64",
    srcs = ["base64.cpp"],
    hdrs = ["base64.h"],
    strip_include_prefix = ".",
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "base64_unit_test",
    srcs = [
        "base64_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/utils:__pkg__"],
    deps = [
        ":base64",
        "@googletest//:gtest_main",
    ],
)

cc_gtest_unit_test(
    name = "scoped_operation_test",
    srcs = ["scoped_operation_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":scoped_operation",
        "@score_baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":base64_unit_test",
        ":scoped_operation_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
    ],
    tidy_config_file = ".clang-tidy-extra",
)
