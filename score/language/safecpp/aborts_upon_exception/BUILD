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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_library(
    name = "abortsuponexception",
    srcs = ["aborts_upon_exception.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
)

cc_test(
    name = "abortsuponexception_lib_test",
    size = "small",
    srcs = [
        "aborts_upon_exception_test.cpp",
    ],
    # Uses throws_upon_exception feature to ensure aborts_upon_exception feature is deactivated
    # on toolchain level so that unmistakably the linked abortsuponexception library gets tested
    features = ["throws_upon_exception"],
    tags = ["unit"],
    deps = [
        ":abortsuponexception",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "abortsuponexception_toolchain_test",
    size = "small",
    srcs = [
        "aborts_upon_exception_test.cpp",
    ],
    features = ["aborts_upon_exception"],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":abortsuponexception_lib_test",
        ":abortsuponexception_toolchain_test",
    ],
    visibility = ["@score_baselibs//score/language/safecpp:__pkg__"],
)
