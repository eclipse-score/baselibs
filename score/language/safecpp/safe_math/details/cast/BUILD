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
    name = "cast",
    srcs = ["cast.cpp"],
    hdrs = ["cast.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/language/safecpp/safe_math:__pkg__",
        "@score_baselibs//score/language/safecpp/safe_math/details/addition_subtraction:__pkg__",
        "@score_baselibs//score/language/safecpp/safe_math/details/division:__pkg__",
        "@score_baselibs//score/language/safecpp/safe_math/details/multiplication:__pkg__",
        "@score_baselibs//score/language/safecpp/safe_math/details/negate:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/safe_math:error",
        "@score_baselibs//score/language/safecpp/safe_math/details/comparison",
        "@score_baselibs//score/result",
    ],
)

cc_gtest_unit_test(
    name = "unit_test",
    srcs = ["cast_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/language/safecpp/safe_math:__pkg__",
    ],
    deps = [
        ":cast",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/safe_math/details:test_type_collection",
    ],
)
