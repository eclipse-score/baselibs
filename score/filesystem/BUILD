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
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "filesystem",
    hdrs = [
        "filesystem.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":struct",
        "@score-baselibs//score/filesystem/factory",
    ],
)

cc_library(
    name = "mock",
    testonly = True,
    srcs = ["filesystem_mock.cpp"],
    hdrs = [
        "filesystem_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":standard_filesystem_fake",
        "@score-baselibs//score/filesystem/factory:fake",
        "@score-baselibs//score/filesystem/file_utils:mock",
        "@score-baselibs//score/filesystem/filestream:fake",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "struct",
    srcs = [
        "filesystem_struct.cpp",
    ],
    hdrs = [
        "filesystem_struct.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [":__subpackages__"],
    deps = [
        ":standard_filesystem",
        "@score-baselibs//score/filesystem/file_utils",
        "@score-baselibs//score/filesystem/filestream",
    ],
)

cc_library(
    name = "path",
    srcs = ["path.cpp"],
    hdrs = ["path.h"],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "@score-baselibs//score/language/futurecpp",
    ],
    tags = ["FFI"],
    visibility = [":__subpackages__"],
)

cc_library(
    name = "error",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [":__subpackages__"],
    deps = ["@score-baselibs//score/result"],
)

cc_library(
    name = "file_status",
    srcs = ["file_status.cpp"],
    hdrs = ["file_status.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [":__subpackages__"],
    deps = ["@score-baselibs//score/os:stat"],
)

cc_library(
    name = "standard_filesystem",
    srcs = [
        "details/standard_filesystem.cpp",
        "i_standard_filesystem.cpp",
        "iterator/directory_entry.cpp",
        "iterator/directory_iterator.cpp",
        "iterator/recursive_directory_iterator.cpp",
    ],
    hdrs = [
        "details/standard_filesystem.h",
        "i_standard_filesystem.h",
        "iterator/directory_entry.h",
        "iterator/directory_iterator.h",
        "iterator/recursive_directory_iterator.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "@score-baselibs//score/language/futurecpp",
    ],
    tags = ["FFI"],
    visibility = [":__subpackages__"],
    deps = [
        ":error",
        ":file_status",
        ":path",
        "@score-baselibs//score/filesystem/filestream",
        "@score-baselibs//score/os:dirent",
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "@score-baselibs//score/os:stdio",
        "@score-baselibs//score/os:stdlib",
        "@score-baselibs//score/os:unistd",
    ],
)

cc_library(
    name = "standard_filesystem_fake",
    testonly = True,
    srcs = [
        "iterator/dirent_fake.cpp",
        "standard_filesystem_fake.cpp",
    ],
    hdrs = [
        "iterator/dirent_fake.h",
        "standard_filesystem_fake.h",
        "standard_filesystem_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        ":__subpackages__",
    ],
    deps = [
        ":filesystem",
        "@score-baselibs//score/filesystem/filestream:fake",
        "@score-baselibs//score/os/mocklib:dirent_mock",
        "@score-baselibs//score/os/mocklib:stat_mock",
        "@score-baselibs//score/os/mocklib:stdlib_mock",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "standard_filesystem_unit_test",
    srcs = [
        "details/standard_filesystem_test.cpp",
        "details/test_helper.cpp",
        "details/test_helper.h",
        "error_test.cpp",
        "file_status_test.cpp",
        "iterator/directory_entry_test.cpp",
        "iterator/directory_iterator_test.cpp",
        "iterator/dirent_fake_test.cpp",
        "iterator/recursive_directory_iterator_test.cpp",
        "path_test.cpp",
        "standard_filesystem_fake_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        ":standard_filesystem",
        ":standard_filesystem_fake",
        "@score-baselibs//score/os/mocklib:dirent_mock",
        "@score-baselibs//score/os/mocklib:stat_mock",
        "@score-baselibs//score/os/mocklib:stdio_mock",
        "@score-baselibs//score/os/mocklib:stdlib_mock",
        "@score-baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        "standard_filesystem_unit_test",
        "@score-baselibs//score/filesystem/factory:unit_test",
        "@score-baselibs//score/filesystem/file_utils:unit_test",
        "@score-baselibs//score/filesystem/filestream:unit_test_basic",
        "@score-baselibs//score/filesystem/filestream:unit_test_extra",
    ],
    visibility = [
        "//visibility:public",
    ],
)
