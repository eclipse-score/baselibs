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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "wait_free_stack",
    hdrs = [
        "wait_free_stack.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__subpackages__",
        "@score-baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/memory/shared:atomic_indirector",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "unit_test",
    srcs = ["wait_free_stack_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":wait_free_stack",
        "@score-baselibs//score/memory/shared:atomic_indirector_mock_binding",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = ["unit_test"],
    visibility = ["@score-baselibs//score/mw/log/detail:__pkg__"],
)
