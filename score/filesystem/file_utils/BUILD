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
    name = "file_utils",
    srcs = [
        "file_utils.cpp",
        "i_file_utils.cpp",
    ],
    hdrs = [
        "file_utils.h",
        "i_file_utils.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/filesystem:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/filesystem:error",
        "@score_baselibs//score/filesystem:path",
        "@score_baselibs//score/filesystem:standard_filesystem",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:grp",
        "@score_baselibs//score/os:object_seam",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/os:stdlib",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "file_test_utils",
    testonly = True,
    srcs = [
        "file_test_utils.cpp",
    ],
    hdrs = [
        "file_test_utils.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
        # "@ddad//platform/aas:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/filesystem:error",
        "@score_baselibs//score/filesystem:path",
    ],
)

cc_library(
    name = "fake",
    testonly = True,
    srcs = ["file_utils_fake.cpp"],
    hdrs = ["file_utils_fake.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/filesystem:__subpackages__"],
    deps = [
        ":mock",
    ],
)

cc_library(
    name = "mock",
    testonly = True,
    srcs = [
    ],
    hdrs = [
        "file_utils_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/filesystem:__subpackages__",
    ],
    deps = [
        ":file_utils",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "file_utils_mock_test.cpp",
        "file_utils_test.cpp",
        "i_file_utils_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/filesystem:__pkg__",
    ],
    deps = [
        ":file_utils",
        ":mock",
        "@score_baselibs//score/filesystem:standard_filesystem_fake",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:grp_mock",
        "@score_baselibs//score/os/mocklib:stat_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)
