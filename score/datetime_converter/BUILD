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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "datetime_converter",
    srcs = ["datetime_converter.cpp"],
    hdrs = ["datetime_converter.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "time_conversion",
    srcs = ["time_conversion.cpp"],
    hdrs = ["time_conversion.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
)

cc_test(
    name = "datetime_converter_test",
    srcs = ["datetime_converter_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/datetime_converter:__pkg__"],
    deps = [
        ":datetime_converter",
        "@googletest//:gtest_main",
    ],
)

cc_gtest_unit_test(
    name = "time_conversion_test",
    srcs = ["time_conversion_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/datetime_converter:__pkg__"],
    deps = [":time_conversion"],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":datetime_converter_test",
        ":time_conversion_test",
    ],
    visibility = ["//visibility:public"],
)
