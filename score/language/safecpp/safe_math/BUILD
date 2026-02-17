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
    name = "safe_math",
    srcs = ["safe_math.cpp"],
    hdrs = ["safe_math.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":error",
        "@score_baselibs//score/language/safecpp/safe_math/details/absolute",
        "@score_baselibs//score/language/safecpp/safe_math/details/addition_subtraction",
        "@score_baselibs//score/language/safecpp/safe_math/details/cast",
        "@score_baselibs//score/language/safecpp/safe_math/details/comparison",
        "@score_baselibs//score/language/safecpp/safe_math/details/division",
        "@score_baselibs//score/language/safecpp/safe_math/details/multiplication",
        "@score_baselibs//score/language/safecpp/safe_math/details/negate",
        "@score_baselibs//score/language/safecpp/safe_math/details/type_traits",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "error",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/safe_math:__subpackages__"],
    deps = ["@score_baselibs//score/result"],
)

cc_gtest_unit_test(
    name = "error_test",
    srcs = ["error_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/mw/com/impl:__subpackages__",
        "//platform/aas/network/tunneling/pdu_handling:__subpackages__",
        "@score_baselibs//score/language/safecpp/safe_math:__pkg__",
        "@score_baselibs//score/memory/shared:__subpackages__",
    ],
    deps = [":error"],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        "@score_baselibs//score/language/safecpp/safe_math:error_test",
        "@score_baselibs//score/language/safecpp/safe_math/details:floating_point_environment_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/absolute:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/addition_subtraction:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/cast:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/comparison:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/division:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/multiplication:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/negate:unit_test",
        "@score_baselibs//score/language/safecpp/safe_math/details/type_traits:unit_test",
    ],
    excluded_tests_filter = [
        # TODO(Ticket-203919): qemu on ubuntu 20.04 without KVM incorrectly supports FPE
        "FloatingPointEnvironmentTest.*",
    ],
    visibility = ["@score_baselibs//score/language/safecpp:__pkg__"],
)
