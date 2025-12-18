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
    name = "recorder_factory",
    srcs = [
        "recorder_factory.cpp",
    ],
    hdrs = [
        "recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":composite_recorder",
        ":recorder_config",
        "@score_baselibs//score/mw/log:recorder_interface",
        "@score_baselibs//score/mw/log/configuration:configuration_parser",
        "@score_baselibs//score/mw/log/detail:empty_recorder",
        "@score_baselibs//score/mw/log/detail:initialization_reporter",
    ],
    alwayslink = True,
)

cc_library(
    name = "recorder_config",
    hdrs = [
        "recorder_config.h",
    ],
    defines = select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": ["KCONSOLE_LOGGING"],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": ["KFILE_LOGGING"],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": ["KREMOTE_LOGGING"],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/flags:Custom_Logging": ["KCUSTOM_LOGGING"],
        "//conditions:default": [],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "@score_baselibs//score/mw/log/detail:empty_recorder_factory",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            "@score_baselibs//score/mw/log/detail/slog",
        ],
    }) + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": [
            "@score_baselibs//score/mw/log/detail/text_recorder:console_recorder_factory",
        ],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": [
            "@score_baselibs//score/mw/log/detail/file_recorder:file_recorder_factory",
        ],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": [
            "@score_baselibs//score/mw/log/detail/data_router:remote_dlt_recorder_factory",
        ],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/flags:Custom_Logging": [
            "@score_baselibs//score/mw/log/flags:custom_recorder_impl",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "log_entry_deserialize",
    srcs = ["log_entry_deserialize.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/pas/logging:__pkg__",
        "//platform/aas/pas/logging/test:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/detail:logging_identifier",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
    ],
)

cc_library(
    name = "helper_functions",
    hdrs = [
        "helper_functions.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
)

cc_library(
    name = "statistics_reporter",
    srcs = [
        "istatistics_reporter.cpp",
        "statistics_reporter.cpp",
    ],
    hdrs = [
        "istatistics_reporter.h",
        "statistics_reporter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_recorder:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
    ],
)

cc_library(
    name = "dlt_content_formatting",
    srcs = [
        "dlt_format.cpp",
    ],
    hdrs = [
        "dlt_format.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_recorder:__pkg__",
    ],
    deps = [
        ":helper_functions",
        "@score_baselibs//score/bitmanipulation",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/memory:string_literal",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/detail:dlt_argument_counter",
        "@score_baselibs//score/mw/log/detail:integer_representation",
        "@score_baselibs//score/mw/log/detail:log_data_types",
    ],
)

cc_library(
    name = "composite_recorder",
    srcs = [
        "composite_recorder.cpp",
    ],
    hdrs = [
        "composite_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log/detail:initialization_reporter",
        "@score_baselibs//score/mw/log/detail:types_and_errors",
    ],
)

cc_test(
    name = "composite_recorder_test",
    srcs = [
        "composite_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":composite_recorder",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:recorder_mock",
    ],
)

cc_test(
    name = "dlt_format_test",
    srcs = [
        "dlt_format_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":dlt_content_formatting",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "log_entry_deserialize_test",
    srcs = [
        "log_entry_deserialize_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":log_entry_deserialize",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "recorder_factory_test",
    srcs = [
        "recorder_factory_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":recorder_factory",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/configuration:target_config_reader_mock",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
    ],
)

cc_test(
    name = "statistics_reporter_test",
    srcs = [
        "statistics_reporter_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":statistics_reporter",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:recorder_mock",
    ],
)

cc_test(
    name = "helper_functions_test",
    srcs = [
        "helper_functions_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":helper_functions",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "runtime_test",
    srcs = [
        ":runtime_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/mw/log:recorder_mock",
        "@score_baselibs//score/mw/log/detail/common:recorder_factory",
    ],
)

cc_test(
    name = "log_stream_test",
    srcs = [
        "@score_baselibs//score/mw/log:log_stream_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/mw/log:recorder_mock",
        "@score_baselibs//score/mw/log/detail/common:recorder_factory",
        "@score_baselibs//score/mw/log/test/my_custom_lib:my_custom_type_mw_log",
    ],
)

cc_test(
    name = "log_stream_factory_test",
    srcs = [
        "@score_baselibs//score/mw/log:log_stream_factory_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/mw/log:recorder_mock",
        "@score_baselibs//score/mw/log/detail:thread_local_guard",
        "@score_baselibs//score/mw/log/detail/common:recorder_factory",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":composite_recorder_test",
        ":dlt_format_test",
        ":log_entry_deserialize_test",
        ":recorder_factory_test",
        ":statistics_reporter_test",
        ":helper_functions_test",
        ":runtime_test",
        ":log_stream_test",
        ":log_stream_factory_test",
    ],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
    ],
)
