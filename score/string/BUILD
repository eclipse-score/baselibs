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

cc_library(
    name = "string_literal",
    hdrs = ["string_literal.h"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "split_string_view",
    srcs = [
        "split_string_view.cpp",
    ],
    hdrs = [
        "split_string_view.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = ["@score_baselibs//score/language/futurecpp"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "string_comparison_adaptor",
    srcs = [
        "string_comparison_adaptor.cpp",
    ],
    hdrs = [
        "string_comparison_adaptor.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":string_literal",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "split_string_view_test",
    srcs = [
        "split_string_view_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":split_string_view",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "string_comparison_adaptor_test",
    srcs = [
        "string_comparison_adaptor_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":string_comparison_adaptor",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)
