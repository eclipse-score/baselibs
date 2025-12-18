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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "text_recorder",
    srcs = [
        "text_message_builder.cpp",
        "text_message_builder.h",
        "text_recorder.cpp",
    ],
    hdrs = [
        "text_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":file_output_backend",
        ":message_builder_interface",
        ":text_content_formatting",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail:dlt_argument_counter",
        "@score_baselibs//score/os/utils:high_resolution_steady_clock",
    ],
)

cc_library(
    name = "text_content_formatting",
    srcs = [
        "text_format.cpp",
    ],
    hdrs = [
        "text_format.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/memory:string_literal",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/detail:dlt_argument_counter",
        "@score_baselibs//score/mw/log/detail:integer_representation",
        "@score_baselibs//score/mw/log/detail:log_data_types",
    ],
)

cc_library(
    name = "console_recorder_factory",
    srcs = [
        "console_recorder_factory.cpp",
    ],
    hdrs = [
        "console_recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":text_recorder",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
    ],
)

cc_library(
    name = "non_blocking_writer",
    srcs = ["non_blocking_writer.cpp"],
    hdrs = ["non_blocking_writer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail:types_and_errors",
        "@score_baselibs//score/os:unistd",
    ],
)

cc_library(
    name = "message_builder_interface",
    srcs = [
        "imessage_builder.cpp",
    ],
    hdrs = [
        "imessage_builder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/mw/log/detail:log_data_types",
    ],
)

cc_library(
    name = "file_output_backend",
    srcs = [
        "file_output_backend.cpp",
        "slot_drainer.cpp",
        "slot_drainer.h",
    ],
    hdrs = [
        "file_output_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":message_builder_interface",
        ":non_blocking_writer",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail:circular_allocator",
        "@score_baselibs//score/os:fcntl",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "file_output_backend_test.cpp",
        "slot_drainer_test.cpp",
        "text_format_test.cpp",
        "text_message_builder_test.cpp",
        "text_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":file_output_backend",
        ":file_output_backend_mocks",
        ":text_recorder",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:backend_mock",
        "@score_baselibs//score/mw/log/detail:circular_allocator",
        "@score_baselibs//score/mw/log/test/console_logging_environment",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@score_baselibs//score/os/utils/mocklib:path_mock",
    ],
)

cc_library(
    name = "file_output_backend_mocks",
    testonly = True,
    hdrs = ["mock/message_builder_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":non_blocking_writer",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":non_blocking_writer_test",
        ":unit_test",
    ],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
    ],
)

cc_test(
    name = "non_blocking_writer_test",
    srcs = ["non_blocking_writer_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":non_blocking_writer",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/test/console_logging_environment",
        "@score_baselibs//score/os/mocklib:unistd_mock",
    ],
)
