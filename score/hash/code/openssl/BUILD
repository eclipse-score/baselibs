# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

cc_library(
    name = "openssl_cal",
    srcs = [
        "openssl_hash_calculator.cpp",
    ],
    hdrs = [
        "openssl_hash_calculator.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/hash/code/common",
        "@score_baselibs//score/hash/code/core",
        "@score_baselibs//score/hash/code/openssl/openssl_wrapper:openssl_lib",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "openssl_hash_calculator_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/hash/code/common:common_test",
        "@score_baselibs//score/hash/code/openssl:openssl_cal",
        "@score_baselibs//score/hash/code/openssl/openssl_wrapper:openssl_lib",
        "@score_baselibs//score/hash/code/openssl/openssl_wrapper:openssl_lib_mock",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":unit_test",
    ],
    visibility = ["//visibility:public"],  # platform_only
)
