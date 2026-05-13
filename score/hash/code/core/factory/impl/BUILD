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
    name = "factory_headers",
    hdrs = [
        "hash_calculator_factory.h",
        "safe_hash_calculator_factory.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/hash/code/core/factory",
        "@score_baselibs//score/hash/code/openssl:openssl_cal",
    ],
)

cc_library(
    name = "hash_impl_ieee",
    srcs = [
        "hash_calculator_factory_ieee.cpp",
    ],
    hdrs = [
        "hash_calculator_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/hash:__pkg__"],
    deps = [
        "@score_baselibs//score/hash/code/core/factory",
        "@score_baselibs//score/hash/code/crc:crc_ieee",
        "@score_baselibs//score/hash/code/openssl:openssl_cal",
        "@score_baselibs//score/hash/code/sha256digest",
    ],
)

cc_library(
    name = "safe_hash_impl_ieee",
    srcs = [
        "safe_hash_calculator_factory_ieee.cpp",
    ],
    hdrs = [
        "safe_hash_calculator_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/hash:__pkg__"],
    deps = [
        "@score_baselibs//score/hash/code/core/factory",
        "@score_baselibs//score/hash/code/crc:crc_ieee",
        "@score_baselibs//score/hash/code/sha256digest",
    ],
)

# Extension point:
# `crc_variant` and `safe_crc_variant` are provided as extension points,
# meaning that one may replace them by custom implementations that would
# provide different polynomials. These extensions may directly inherit from:
# //score/hash/code/crc:crc_ieee
# As that target provides a generic implementation of CRC, the custom
# implementations would then only need to provide their polynomials
# and override these two targets on their .bazelrc files.
label_flag(
    name = "crc_variant",
    build_setting_default = ":hash_impl_ieee",
    visibility = [
        "@score_baselibs//score/hash:__pkg__",
    ],
)

label_flag(
    name = "safe_crc_variant",
    build_setting_default = ":safe_hash_impl_ieee",
    visibility = [
        "@score_baselibs//score/hash:__pkg__",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "hash_calculator_factory_ieee_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = ["@score_baselibs//score/hash/code/core:__pkg__"],
    deps = [
        ":hash_impl_ieee",
        ":safe_hash_impl_ieee",
        "@googletest//:gtest_main",
        "@score_baselibs//score/hash/code/openssl/openssl_wrapper:openssl_lib_mock",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [":unit_test"],
    visibility = ["//visibility:public"],  # platform_only
)
