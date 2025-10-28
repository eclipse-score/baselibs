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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "acl",
    srcs = [
        "access_control_list.cpp",
        "i_access_control_list.cpp",
    ],
    hdrs = [
        "access_control_list.h",
        "i_access_control_list.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/os:acl",
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/result",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "mock",
    srcs = ["access_control_list_mock.cpp"],
    hdrs = [
        "access_control_list_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":acl",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = ["access_control_list_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/os/utils/test:__pkg__",
    ],
    deps = [
        ":acl",
        "@score_baselibs//score/os/mocklib:acl_mock",
        "@googletest//:gtest_main",
    ],
)
