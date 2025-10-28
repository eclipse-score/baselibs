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
    name = "factory",
    srcs = [
        "filesystem_factory.cpp",
        "i_filesystem_factory.cpp",
    ],
    hdrs = [
        "filesystem_factory.h",
        "i_filesystem_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/filesystem:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/filesystem:struct",
    ],
)

cc_library(
    name = "fake",
    testonly = True,
    srcs = [
        "filesystem_factory_fake.cpp",
        "filesystem_factory_mock_objects.cpp",
    ],
    hdrs = [
        "filesystem_factory_fake.h",
        "filesystem_factory_mock.h",
        "filesystem_factory_mock_objects.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/filesystem:__subpackages__",
    ],
    deps = [
        ":factory",
        "@score_baselibs//score/filesystem:standard_filesystem_fake",
        "@score_baselibs//score/filesystem/file_utils:fake",
        "@score_baselibs//score/filesystem/filestream:fake",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "filesystem_factory_fake_test.cpp",
        "filesystem_factory_mock_objects_test.cpp",
        "filesystem_factory_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/filesystem:__pkg__",
    ],
    deps = [
        ":factory",
        ":fake",
        "@googletest//:gtest_main",
    ],
)
