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
    name = "expected",
    srcs = ["expected.cpp"],
    hdrs = ["expected.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/result:__pkg__"],
)

cc_library(
    name = "test_types",
    testonly = True,
    srcs = ["test_types.cpp"],
    hdrs = ["test_types.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
)

cc_library(
    name = "test_type_traits",
    testonly = True,
    srcs = ["test_type_traits.cpp"],
    hdrs = ["test_type_traits.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
)

cc_gtest_unit_test(
    name = "test_type_traits_test",
    srcs = [
        "test_type_traits_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [":test_type_traits"],
)

cc_gtest_unit_test(
    name = "expected_test",
    srcs = [
        "expected_assignment_test.cpp",
        "expected_constructor_test.cpp",
        "expected_equality_test.cpp",
        "expected_monadic_operator_test.cpp",
        "expected_observer_test.cpp",
        "expected_type_trait_test.cpp",
        "unexpected_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "-aborts_upon_exception",
    ],
    deps = [
        ":expected",
        ":test_type_traits",
        ":test_types",
    ],
)

cc_library(
    name = "extensions",
    srcs = ["extensions.cpp"],
    hdrs = ["extensions.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/result:__pkg__"],
    deps = [
        ":expected",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "extensions_test",
    srcs = ["extensions_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":extensions",
        ":test_types",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":expected_test",
        ":extensions_test",
        ":test_type_traits_test",
    ],
    visibility = [
        "@score_baselibs//score/result:__pkg__",
    ],
)
