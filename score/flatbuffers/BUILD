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
        "details/buffer_version_info.cpp",
        "details/error.cpp",
        "details/load_buffer.cpp",
        "details/load_buffer_internal.hpp",
        "details/version_reader.cpp",
        "details/version_reader_impl.hpp",
    ],
    hdrs = [
        "buffer_version_info.hpp",
        "error.hpp",
        "i_version_reader.hpp",
        "load_buffer.hpp",
        "version_reader.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//score/flatbuffers:flatbufferscpp",
        "//score/flatbuffers/common:buffer_version_envelope",
        "//score/flatbuffers/common:buffer_version_generated_bare",
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/result",
    ],
)

cc_test(
    name = "buffer_version_info_unit_test",
    size = "small",
    srcs = ["details/buffer_version_info_test.cpp"],
    tags = ["unit"],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "error_unit_test",
    size = "small",
    srcs = ["details/error_test.cpp"],
    tags = ["unit"],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "idl_parser_unit_test",
    size = "small",
    srcs = ["details/idl_parser_test.cpp"],
    tags = ["unit"],
    deps = [
        ":flatbufferscpp",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "load_buffer_unit_test",
    size = "small",
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
    name = "version_reader_unit_test",
    size = "small",
    srcs = ["details/version_reader_test.cpp"],
    tags = ["unit"],
    deps = [
        ":flatbufferutils",
        "//score/flatbuffers/common:buffer_version_envelope",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "load_buffer_test",
    size = "small",
    srcs = ["test/load_buffer_test.cpp"],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "flatbuffers_cpp_library_test",
    testonly = True,
    srcs = [
        "details/flatfbuffer_allocator_test.cpp",
    ],
    deps = [
        ":flatbufferscpp",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
    ],
)

cc_test(
    name = "version_reader_test",
    size = "small",
    srcs = ["test/version_reader_test.cpp"],
    data = [
        "//score/flatbuffers/test/testdata:gen_versioned_buffer_bin",
        "//score/flatbuffers/test/testdata:gen_versioned_buffer_explicit_ids_bin",
        "//score/flatbuffers/test/testdata:gen_versioned_buffer_version_info_wrong_field",
        "//score/flatbuffers/test/testdata:gen_versioned_buffer_version_info_wrong_field_similar_type",
        "//score/flatbuffers/test/testdata:gen_versioned_max_buffer_bin",
        "//score/flatbuffers/test/testdata:gen_versioned_max_range_buffer_bin",
        "//score/flatbuffers/test/testdata:gen_versioned_min_buffer_bin",
        "//score/flatbuffers/test/testdata:gen_versioned_min_range_buffer_bin",
    ],
    deps = [
        ":flatbufferutils",
        "@googletest//:gtest_main",
    ],
)
