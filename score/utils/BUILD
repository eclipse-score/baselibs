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
    name = "scoped_operation",
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
    srcs = ["test/scoped_operation_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":scoped_operation"],
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
