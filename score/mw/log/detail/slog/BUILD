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

load("@score_baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")
load("//score/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "slog_backend",
    srcs = [
        "slog_backend.cpp",
    ],
    hdrs = [
        "slog_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail:circular_allocator",
        "@score_baselibs//score/mw/log/detail:initialization_reporter",
        "@score_baselibs//score/os/qnx:slog2",
    ],
)

cc_library(
    name = "slog",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":slog_backend",
        ":slog_recorder_factory",
    ],
)

cc_library(
    name = "slog_recorder_factory",
    srcs = [
        "slog_recorder_factory.cpp",
    ],
    hdrs = [
        "slog_recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":slog_backend",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
        "@score_baselibs//score/mw/log/detail/text_recorder",
    ],
)

cc_test(
    name = "slog_recorder_factory_test",
    srcs = [
        "slog_recorder_factory_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":slog_recorder_factory",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/mw/log/configuration",
    ],
)

cc_test(
    name = "slog_backend_test",
    srcs = [
        "slog_backend_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
    ],
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/mw/log/configuration",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            ":slog",
            "@score_baselibs//score/os/mocklib/qnx:slog2_mock",
        ],
    }),
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":slog_backend_test",
        ":slog_recorder_factory_test",
    ],
    visibility = ["@score_baselibs//score/mw/log/detail:__pkg__"],
)

test_suite(
    name = "unit_tests_host",
    tests = [],
    visibility = ["//visibility:public"],
)
