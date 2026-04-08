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
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

cc_library(
    name = "flag_owner",
    srcs = ["flag_owner.cpp"],
    hdrs = ["flag_owner.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "scope_exit",
    srcs = ["scope_exit.cpp"],
    hdrs = ["scope_exit.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":flag_owner",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "flag_owner_test",
    srcs = ["flag_owner_test.cpp"],
    deps = [
        ":flag_owner",
    ],
)

cc_gtest_unit_test(
    name = "scope_exit_test",
    srcs = ["scope_exit_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":scope_exit",
        "@score_baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score_baselibs//score/language/safecpp/scoped_function:scope",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":flag_owner_test",
        ":scope_exit_test",
    ],
    visibility = ["//platform/aas/lib:__pkg__"],
)

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
    ],
    tidy_config_file = ".clang-tidy-extra",
)
