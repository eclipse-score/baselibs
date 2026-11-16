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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "string_registry",
    srcs = [
        "string_registry.cpp",
    ],
    hdrs = [
        "string_registry.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [":registry_string_view"],
)

cc_gtest_unit_test(
    name = "string_registry_unit_test",
    srcs = [
        "string_registry_tests.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    deps = [
        ":string_registry",
    ],
)

cc_library(
    name = "registry_string_view",
    hdrs = [
        "registry_string_view.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_unit_test(
    name = "registry_string_view_unit_tests",
    srcs = [
        "registry_string_view_tests.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    deps = [
        ":registry_string_view",
        ":string_registry",
        "@googletest//:gtest_main",
    ],
)
