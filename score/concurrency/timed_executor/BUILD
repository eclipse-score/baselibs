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
    name = "timed_task",
    srcs = ["timed_task.cpp"],
    hdrs = ["timed_task.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/concurrency:executor",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "periodic_task",
    srcs = ["periodic_task.cpp"],
    hdrs = ["periodic_task.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":timed_task",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "periodic_task_tests",
    srcs = ["periodic_task_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":periodic_task",
        "@score_baselibs//score/concurrency:clock",
        "@score_baselibs//score/concurrency:executor_mock",
    ],
)

cc_library(
    name = "delayed_task",
    srcs = ["delayed_task.cpp"],
    hdrs = ["delayed_task.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":timed_task",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "delayed_task_tests",
    srcs = ["delayed_task_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":delayed_task",
        "@score_baselibs//score/concurrency:clock",
        "@score_baselibs//score/concurrency:executor_mock",
    ],
)

cc_library(
    name = "timed_executor",
    srcs = ["timed_executor.cpp"],
    hdrs = ["timed_executor.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":delayed_task",
        ":periodic_task",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "concurrent_timed_executor",
    srcs = ["concurrent_timed_executor.cpp"],
    hdrs = ["concurrent_timed_executor.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":timed_executor",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "timed_executor_mock",
    testonly = True,
    srcs = ["timed_executor_mock.cpp"],
    hdrs = ["timed_executor_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":timed_executor",
        "@googletest//:gtest",
    ],
)

cc_gtest_unit_test(
    name = "concurrent_timed_executor_test",
    srcs = ["concurrent_timed_executor_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":concurrent_timed_executor",
        "@score_baselibs//score/concurrency:clock",
        "@score_baselibs//score/concurrency:executor_mock",
        "@score_baselibs//score/concurrency:thread_pool",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":periodic_task_tests",
        ":delayed_task_tests",
        ":concurrent_timed_executor_test",
    ],
    visibility = ["@score_baselibs//score/concurrency:__pkg__"],
)
