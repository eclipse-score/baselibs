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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "error",
    srcs = [
        "error.cpp",
        "error_code.cpp",
        "error_domain.cpp",
    ],
    hdrs = [
        "error.h",
        "error_code.h",
        "error_domain.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "result",
    hdrs = ["result.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":error",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/result/details/expected",
        "@score_baselibs//score/result/details/expected:extensions",
    ],
)

cc_library(
    name = "result_for_rust",
    hdrs = ["result.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":result",
        # stdlib abi guard not available in score
    ],
)

cc_library(
    name = "error_msg_mapping",
    srcs = ["error_msg_mapping.cpp"],
    hdrs = ["error_msg_mapping.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/result:__subpackages__",
    ],
    deps = [
        ":error",
    ],
)

cc_gtest_unit_test(
    name = "error_msg_mapping_test",
    srcs = [
        "dummy_error_code.h",
        "error_msg_mapping_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":error_msg_mapping",
        ":result",
    ],
)

cc_gtest_unit_test(
    name = "tests",
    srcs = [
        "dummy_error_code.h",
        "error_test.cpp",
        "result_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":error",
        ":result",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":error_msg_mapping_test",
        ":tests",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/result/details/expected:unit_test_suite",
    ],
    visibility = [
        "//visibility:public",
    ],
)
