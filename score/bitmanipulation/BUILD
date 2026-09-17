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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_docs_as_code//:docs.bzl", "docs_bundle")

cc_test(
    name = "bit_manipulation_test",
    srcs = ["bit_manipulation_test.cpp"],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":bitmanipulation",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "bitmask_operators_test",
    srcs = ["bitmask_operators_test.cpp"],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":bitmask_operators",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bitmanipulation",
    srcs = ["bit_manipulation.cpp"],
    hdrs = ["bit_manipulation.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

cc_library(
    name = "bitmask_operators",
    srcs = ["bitmask_operators.cpp"],
    hdrs = ["bitmask_operators.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

docs_bundle(
    name = "docs",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)
