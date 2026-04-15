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

# Backend plugin registrants for non-default log backends (file, remote, slog).
# Each registrant is an additive Bazel dep that registers its creator function
# into the global backend table during static initialization.

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

# Plugin: DLT File Logging
# Add this dep to your binary to enable file-based DLT logging.
cc_library(
    name = "file",
    srcs = select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": ["file_registrant.cpp"],
        "//conditions:default": [],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/mw/log:minimal",
    ] + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": [
            "@score_baselibs//score/mw/log/detail/file_recorder:file_recorder_factory",
        ],
        "//conditions:default": [],
    }),
    alwayslink = True,
)

# Plugin: Remote DLT via DataRouter
# Add this dep to your binary to enable remote DLT logging.
cc_library(
    name = "remote",
    srcs = select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": ["remote_registrant.cpp"],
        "//conditions:default": [],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/mw/log:minimal",
    ] + select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": [
            "@score_baselibs//score/mw/log/detail/data_router:remote_dlt_recorder_factory",
        ],
        "//conditions:default": [],
    }),
    alwayslink = True,
)

# Plugin: QNX slog2 System Logging
# Automatically included on QNX builds.
cc_library(
    name = "slog",
    srcs = ["slog_registrant.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/mw/log:minimal",
        "@score_baselibs//score/mw/log/detail/slog:slog_recorder_factory",
    ],
    alwayslink = True,
)

# Plugin: Custom user-provided Logging backend
# Opt-in. Build with --@score_baselibs//score/mw/log/flags:KCustom_Logging=True
# and --@score_baselibs//score/mw/log/flags:custom_recorder_impl=//your:target.
cc_library(
    name = "custom",
    srcs = select({
        "@score_baselibs//score/mw/log/flags:Custom_Logging": ["custom_registrant.cpp"],
        "//conditions:default": [],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/mw/log:minimal",
    ] + select({
        "@score_baselibs//score/mw/log/flags:Custom_Logging": [
            "@score_baselibs//score/mw/log/flags:custom_recorder_impl",
        ],
        "//conditions:default": [],
    }),
    alwayslink = True,
)

cc_test(
    name = "file_registrant_test",
    srcs = select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": ["file_registrant_enabled_test.cpp"],
        "//conditions:default": ["file_registrant_disabled_test.cpp"],
    }),
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":file",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_table",
    ],
)

cc_test(
    name = "remote_registrant_test",
    srcs = select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": ["remote_registrant_enabled_test.cpp"],
        "//conditions:default": ["remote_registrant_disabled_test.cpp"],
    }),
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":remote",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_table",
        "@score_baselibs//score/mw/log:console",
    ],
)

cc_test(
    name = "slog_registrant_test",
    srcs = ["slog_registrant_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        ":slog",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_table",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":file_registrant_test",
        ":remote_registrant_test",
        ":slog_registrant_test",
    ],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
    ],
)
