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
    name = "atomic_interface",
    hdrs = ["i_atomic.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [],
)

cc_library(
    name = "atomic_indirector",
    srcs = ["atomic_indirector.cpp"],
    hdrs = ["atomic_indirector.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [":atomic_interface"],
)

cc_library(
    name = "atomic_indirector_mock_binding",
    testonly = True,
    srcs = ["atomic_mock.cpp"],
    hdrs = ["atomic_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        ":atomic_indirector",
        "@googletest//:gtest",
    ],
)

cc_gtest_unit_test(
    name = "atomic_indirector_test",
    srcs = ["atomic_indirector_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/concurrency:__pkg__",
    ],
    deps = [
        ":atomic_indirector",
        ":atomic_indirector_mock_binding",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":atomic_indirector_test",
    ],
    visibility = [
        "@score_baselibs//score/concurrency:__pkg__",
    ],
)
