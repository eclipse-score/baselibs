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
    name = "future",
    srcs = [
        "base_interruptible_future.cpp",
        "base_interruptible_promise.cpp",
        "base_interruptible_state.cpp",
        "error.cpp",
        "interruptible_future.cpp",
        "interruptible_promise.cpp",
        "interruptible_shared_future.cpp",
        "interruptible_state.cpp",
    ],
    hdrs = [
        "base_interruptible_future.h",
        "base_interruptible_promise.h",
        "base_interruptible_state.h",
        "error.h",
        "interruptible_future.h",
        "interruptible_promise.h",
        "interruptible_shared_future.h",
        "interruptible_state.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score_baselibs//score/language/safecpp/scoped_function:scope",
        "@score_baselibs//score/concurrency:interruptible_wait",
        "@score_baselibs//score/result",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "test_types",
    srcs = ["test_types.cpp"],
    hdrs = ["test_types.h"],
    features = COMPILER_WARNING_FEATURES,
)

cc_gtest_unit_test(
    name = "unit_test",
    srcs = [
        "base_interruptible_future_test.cpp",
        "base_interruptible_promise_test.cpp",
        "base_interruptible_state_test.cpp",
        "error_test.cpp",
        "interruptible_future_test.cpp",
        "interruptible_promise_test.cpp",
        "interruptible_shared_future_test.cpp",
        "interruptible_state_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":future",
        ":test_types",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":unit_test",
    ],
    visibility = ["@score_baselibs//score/concurrency:__pkg__"],
)
