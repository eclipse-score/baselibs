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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "interprocess_mutex",
    srcs = ["interprocess_mutex.cpp"],
    hdrs = ["interprocess_mutex.h"],
    features = COMPILER_WARNING_FEATURES,
    # ToDo: Remove the linkopts if clarified with score toolchain (needed after boost dep update)
    linkopts = ["-lpthread"],
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/concurrency:__pkg__",
        "@score-baselibs//score/memory/shared:__pkg__",
        "@score-baselibs//score/memory/shared/test/sct:__pkg__",
        "@score-baselibs//score/os/utils/test:__pkg__",
        # "@ddad@score-baselibs//score/os/utils/interprocess:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "interprocess_conditional_variable",
    srcs = ["interprocess_conditional_variable.cpp"],
    hdrs = ["interprocess_conditional_variable.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/concurrency:__pkg__",
        "@score-baselibs//score/os/utils/test:__pkg__",
        # "@ddad@score-baselibs//score/os/utils/interprocess:__subpackages__",
    ],
    deps = [
        ":interprocess_mutex",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "interprocess_notification",
    srcs = ["interprocess_notification.cpp"],
    hdrs = ["interprocess_notification.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/mw/com/test:__subpackages__",
        # "@ddad@score-baselibs//score/os/utils/interprocess:__subpackages__",
    ],
    deps = [
        ":interprocess_conditional_variable",
        ":interprocess_mutex",
        "@score-baselibs//score/concurrency",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":interprocess_conditional_variable_test",
        ":interprocess_notification_test",
    ],
    visibility = [
        "@score-baselibs//score/os/utils/test:__subpackages__",
        # "@ddad@score-baselibs//score/os/utils:__subpackages__",
    ],
)

cc_gtest_unit_test(
    name = "interprocess_conditional_variable_test",
    srcs = ["interprocess_conditional_variable_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        # "@ddad@score-baselibs//score/os/utils/interprocess:__subpackages__",
    ],
    deps = [":interprocess_conditional_variable"],
)

cc_gtest_unit_test(
    name = "interprocess_notification_test",
    srcs = ["interprocess_notification_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        # "@ddad@score-baselibs//score/os/utils/interprocess:__subpackages__",
    ],
    deps = [":interprocess_notification"],
)
