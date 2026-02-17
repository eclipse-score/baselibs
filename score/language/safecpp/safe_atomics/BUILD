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
    name = "try_atomic_add",
    srcs = ["try_atomic_add.cpp"],
    hdrs = ["try_atomic_add.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":error",
        "@score_baselibs//score/language/safecpp/safe_math/details/addition_subtraction",
        "@score_baselibs//score/memory/shared:atomic_indirector",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "error",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/safe_atomics:__subpackages__"],
    deps = ["@score_baselibs//score/result"],
)

cc_gtest_unit_test(
    name = "try_atomic_add_test",
    srcs = ["try_atomic_add_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":try_atomic_add",
        "@score_baselibs//score/language/safecpp/safe_math/details:test_type_collection",
        "@score_baselibs//score/memory/shared:atomic_indirector_mock_binding",
    ],
)

cc_gtest_unit_test(
    name = "error_test",
    srcs = ["error_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":error"],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":try_atomic_add_test",
        ":error_test",
    ],
    visibility = ["@score_baselibs//score/language/safecpp:__pkg__"],
)
