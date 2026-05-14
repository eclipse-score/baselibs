# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "flatbufferscpp",
    srcs = [
        "details/idl_parser.cpp",
    ],
    hdrs = [
        "@flatbuffers//:include/flatbuffers/allocator.h",
        "@flatbuffers//:include/flatbuffers/array.h",
        "@flatbuffers//:include/flatbuffers/base.h",
        "@flatbuffers//:include/flatbuffers/buffer.h",
        "@flatbuffers//:include/flatbuffers/buffer_ref.h",
        "@flatbuffers//:include/flatbuffers/default_allocator.h",
        "@flatbuffers//:include/flatbuffers/detached_buffer.h",
        "@flatbuffers//:include/flatbuffers/flatbuffer_builder.h",
        "@flatbuffers//:include/flatbuffers/flatbuffers.h",
        "@flatbuffers//:include/flatbuffers/stl_emulation.h",
        "@flatbuffers//:include/flatbuffers/string.h",
        "@flatbuffers//:include/flatbuffers/struct.h",
        "@flatbuffers//:include/flatbuffers/table.h",
        "@flatbuffers//:include/flatbuffers/vector.h",
        "@flatbuffers//:include/flatbuffers/vector_downward.h",
        "@flatbuffers//:include/flatbuffers/verifier.h",
    ],
    strip_include_prefix = "/include",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "flatbufferutils",
    srcs = [
        "details/load_buffer.cpp",
        "details/load_buffer_internal.hpp",
    ],
    hdrs = [
        "load_buffer.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/result",
    ],
)

cc_test(
    name = "load_buffer_unit_test",
    srcs = ["details/load_buffer_test.cpp"],
    tags = ["unit"],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:stat_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
    ],
)

cc_test(
    name = "load_buffer_test",
    srcs = ["test/load_buffer_test.cpp"],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
    ],
)
