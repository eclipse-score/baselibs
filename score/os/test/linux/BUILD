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
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

test_suite(
    name = "unit_tests_linux",
    tests = [
        ":pthread_test",
        ":unistd_test",
    ],
    visibility = ["//platform/aas/lib:__pkg__"],
)

cc_test(
    name = "unistd_test",
    srcs = ["unistd_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "unit",
    ],
    target_compatible_with = ["@platforms//os:linux"],
    deps = [
        "@score-baselibs//score/os/linux",
        "@score-baselibs//score/os/linux:linux_header",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pthread_test",
    srcs = [
        "pthread_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "unit",
    ],
    target_compatible_with = ["@platforms//os:linux"],
    deps = [
        "@score-baselibs//score/os:pthread",
        "@googletest//:gtest_main",
    ],
)
