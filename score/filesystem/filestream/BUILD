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
    name = "filestream",
    srcs = [
        "file_buf.cpp",
        "file_buf.h",
        "file_factory.cpp",
        "file_stream.cpp",
        "i_file_factory.cpp",
        "stdio_filebuf_base.h",
    ],
    hdrs = [
        "file_factory.h",
        "file_stream.h",
        "i_file_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES + ["throws_upon_exception"],
    implementation_deps = [
        "@score_baselibs//score/filesystem:file_status",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:stdio",
        "@score_baselibs//score/os:unistd",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/filesystem:__subpackages__",
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/filesystem:error",
        "@score_baselibs//score/filesystem:path",
        "@score_baselibs//score/os:object_seam",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/result",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "fake",
    testonly = True,
    srcs = [
        "bad_string_stream_collection.cpp",
        "file_factory_fake.cpp",
        "i_string_stream_collection.cpp",
        "simple_string_stream_collection.cpp",
    ],
    hdrs = [
        "bad_string_stream_collection.h",
        "file_factory_fake.h",
        "file_factory_mock.h",
        "i_string_stream_collection.h",
        "simple_string_stream_collection.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/filesystem:__subpackages__"],
    deps = [
        ":filestream",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test_basic",
    srcs = [
        "file_factory_fake_test.cpp",
        "file_factory_test.cpp",
        "i_file_factory_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/filesystem:__pkg__",
    ],
    deps = [
        ":fake",
        ":filestream",
        "@score_baselibs//score/filesystem/factory",
        "@score_baselibs//score/filesystem/file_utils:file_test_utils",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:stat_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)

# Special tests for error conditions
cc_test(
    name = "unit_test_extra",
    srcs = [
        "file_buf.cpp",
        "file_buf.h",
        "file_buf_test.cpp",
        "file_factory.cpp",
        "file_factory.h",
        "file_factory_test_extra.cpp",
        "file_stream.cpp",
        "file_stream.h",
        "i_file_factory.cpp",
        "i_file_factory.h",
        "stdio_filebuf_base_testing.h",
    ],
    defines = [
        "STDIO_FILEBUF_BASE_TESTING",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/filesystem:__pkg__",
        # "@ddad@score_baselibs//score/filesystem:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/filesystem:error",
        "@score_baselibs//score/filesystem:file_status",
        "@score_baselibs//score/filesystem:path",
        "@score_baselibs//score/filesystem/file_utils:file_test_utils",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:object_seam",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/os:stdio",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:stdio_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@score_baselibs//score/result",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)
