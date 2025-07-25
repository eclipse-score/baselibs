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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_gtest_unit_test")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "comparison",
    srcs = ["comparison.cpp"],
    hdrs = ["comparison.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/language/safecpp/safe_math:__pkg__",
        "@score-baselibs//score/language/safecpp/safe_math/details/absolute:__pkg__",
        "@score-baselibs//score/language/safecpp/safe_math/details/addition_subtraction:__pkg__",
        "@score-baselibs//score/language/safecpp/safe_math/details/cast:__pkg__",
        "@score-baselibs//score/language/safecpp/safe_math/details/multiplication:__pkg__",
        "@score-baselibs//score/language/safecpp/safe_math/details/negate:__pkg__",
    ],
    deps = [
        "@score-baselibs//score/language/safecpp/safe_math:error",
        "@score-baselibs//score/language/safecpp/safe_math/details/type_traits",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "unit_test",
    srcs = ["comparison_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score-baselibs//score/language/safecpp/safe_math:__pkg__",
    ],
    deps = [
        ":comparison",
        "@score-baselibs//score/language/safecpp/safe_math/details:test_type_collection",
        "@score-baselibs//score/language/futurecpp",
    ],
)
