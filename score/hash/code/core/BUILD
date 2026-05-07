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
    name = "core",
    srcs = [
        "hash.cpp",
        "i_hash_calculator.cpp",
        "typed_hash.cpp",
    ],
    hdrs = [
        "hash.h",
        "i_hash_calculator.h",
        "typed_hash.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/hash:__subpackages__"],
    deps = [
        "@score_baselibs//score/hash/code/common",
        "@score_baselibs//score/json",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:frontend",
    ],
)

cc_library(
    name = "hash_cal_mock",
    srcs = [],
    hdrs = [
        "hash_calculator_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/hash:__subpackages__"],
    deps = [
        "@score_baselibs//score/hash/code/common",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "hash_test.cpp",
        "typed_hash_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        ":core",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":unit_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/hash/code/core/factory/impl:unit_tests",
    ],
    visibility = ["//visibility:public"],  # platform_only
)
