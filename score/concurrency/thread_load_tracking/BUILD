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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "thread_load_tracking",
    srcs = [
        "thread_load_tracking.cpp",
        "thread_load_tracking_token.cpp",
    ],
    hdrs = [
        "thread_load_tracking.h",
        "thread_load_tracking_state.h",
        "thread_load_tracking_token.h",
        "work_load.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "thread_load_tracking_test",
    srcs = [
        "thread_load_tracking_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":thread_load_tracking",
    ],
)
