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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "floating_point_environment",
    srcs = ["floating_point_environment.cpp"],
    hdrs = ["floating_point_environment.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/safe_math/details:__subpackages__"],
    deps = [
        "@score_baselibs//score/language/safecpp/safe_math:error",
        "@score_baselibs//score/language/safecpp/safe_math/details/comparison",
        "@score_baselibs//score/result",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "floating_point_environment_test",
    srcs = ["floating_point_environment_test.cpp"],
    # Required to know in code if the test is running under memcheck
    features = COMPILER_WARNING_FEATURES + ["memcheck_macro"],
    visibility = [
        "@score_baselibs//score/language/safecpp/safe_math:__pkg__",
    ],
    deps = [
        ":floating_point_environment",
        "@score_baselibs//score/language/safecpp/safe_math:error",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "test_type_collection",
    testonly = True,
    srcs = ["test_type_collection.cpp"],
    hdrs = ["test_type_collection.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/language/safecpp/safe_math/details:__subpackages__"],
    deps = ["@googletest//:gtest"],
)
