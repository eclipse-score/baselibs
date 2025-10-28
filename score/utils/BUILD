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
    name = "time_conversion",
    srcs = ["time_conversion.cpp"],
    hdrs = ["time_conversion.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/os/utils:__subpackages__",
    ],
)

cc_library(
    name = "pimpl_ptr",
    srcs = ["pimpl_ptr.cpp"],
    hdrs = ["pimpl_ptr.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/os/utils:__subpackages__",
    ],
)

cc_library(
    name = "string_hash",
    srcs = ["string_hash.cpp"],
    hdrs = ["string_hash.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/os/utils:__subpackages__",
    ],
)

cc_library(
    name = "payloadvalidation",
    srcs = ["src/PayloadValidation.cpp"],
    hdrs = ["src/PayloadValidation.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "scoped_operation",
    hdrs = ["src/scoped_operation.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "pimpl_ptr_test",
    srcs = ["pimpl_ptr_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":pimpl_ptr"],
)

cc_gtest_unit_test(
    name = "time_conversion_test",
    srcs = ["time_conversion.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":time_conversion"],
)

cc_gtest_unit_test(
    name = "payloadvalidation_test",
    srcs = ["test/payloadvalidationtest.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":payloadvalidation"],
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
        ":pimpl_ptr_test",
        ":time_conversion_test",
        ":payloadvalidation_test",
        ":scoped_operation_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)
