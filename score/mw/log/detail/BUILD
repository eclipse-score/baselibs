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
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":composite_recorder",
        ":empty_recorder",
        ":initialization_reporter",
        ":recorder_config",
        "@score_baselibs//score/mw/log:recorder_interface",
        "@score_baselibs//score/mw/log/configuration:configuration_parser",
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
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":empty_recorder_factory",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            "@score_baselibs//score/mw/log/detail/slog",
        ],
    }) + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": [
            "@score_baselibs//score/mw/log/detail/file_logging:console_recorder_factory",
        ],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KFile_Logging": [
            "@score_baselibs//score/mw/log/detail/file_logging:file_recorder_factory",
        ],
        "//conditions:default": [],
    }) + select({
        "@score_baselibs//score/mw/log/flags:Remote_Logging": [
            "@score_baselibs//score/mw/log/detail/data_router:remote_dlt_recorder_factory",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "log_recorder_factory",
    srcs = [
        "log_recorder_factory.hpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "empty_recorder_factory",
    srcs = [
        "empty_recorder_factory.cpp",
    ],
    hdrs = [
        "empty_recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":empty_recorder",
        ":initialization_reporter",
        ":log_recorder_factory",
    ],
)

cc_library(
    name = "backend_interface",
    srcs = ["backend.cpp"],
    hdrs = ["backend.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
        "@score_baselibs//score/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":log_data_types",
        "@score_baselibs//score/mw/log:recorder",
    ],
)

cc_library(
    name = "backend_mock",
    testonly = True,
    srcs = ["backend_mock.cpp"],
    hdrs = ["backend_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "logging_identifier",
    srcs = [
        "logging_identifier.cpp",
    ],
    hdrs = [
        "logging_identifier.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "types_and_errors",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
        "@score_baselibs//score/mw/log/configuration:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "log_entry_deserialize",
    srcs = ["log_entry_deserialize.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/pas/logging:__pkg__",
        "//platform/aas/pas/logging/test:__pkg__",
    ],
    deps = [
        ":logging_identifier",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
        "@score_baselibs//score/mw/log:shared_types",
    ],
)

cc_library(
    name = "log_entry",
    srcs = ["log_entry.cpp"],
    hdrs = ["log_entry.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
    ],
    deps = [
        ":logging_identifier",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
        "@score_baselibs//score/mw/log:shared_types",
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
    name = "log_data_types",
    srcs = [
        "log_record.cpp",
        "verbose_payload.cpp",
    ],
    hdrs = [
        "log_record.h",
        "verbose_payload.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":helper_functions",
        ":log_entry",
    ],
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
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
    ],
)

cc_library(
    name = "dlt_argument_counter",
    srcs = ["dlt_argument_counter.cpp"],
    hdrs = [
        "add_argument_result.h",
        "dlt_argument_counter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "integer_representation",
    srcs = [
        "integer_representation.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
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
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":dlt_argument_counter",
        ":helper_functions",
        ":integer_representation",
        ":log_data_types",
        "@score_baselibs//score/bitmanipulation",
        "@score_baselibs//score/memory:string_literal",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "initialization_reporter",
    srcs = [
        "initialization_reporter.cpp",
    ],
    hdrs = [
        "initialization_reporter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
        "@score_baselibs//score/mw/log/configuration:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
        "@score_baselibs//score/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":types_and_errors",
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
        ":initialization_reporter",
        ":types_and_errors",
        "@score_baselibs//score/mw/log:recorder",
    ],
)

cc_library(
    name = "empty_recorder",
    srcs = [
        "empty_recorder.cpp",
    ],
    hdrs = [
        "empty_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":log_data_types",
        ":log_entry",
        ":logging_identifier",
        ":types_and_errors",
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "thread_local_guard",
    srcs = ["thread_local_guard.cpp"],
    hdrs = ["thread_local_guard.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)

cc_library(
    name = "recorder_factory_stub",
    srcs = [
        "recorder_factory_stub.cpp",
    ],
    hdrs = [
        "recorder_factory_stub.h",
    ],
    defines = ["KUSE_STUB_IMPLEMENTATION_ONLY"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        ":empty_recorder",
        "@score_baselibs//score/mw/log:recorder_interface",
        "@score_baselibs//score/mw/log/detail/file_logging:text_recorder",
    ],
    alwayslink = True,
)

cc_library(
    name = "circular_allocator",
    srcs = ["circular_allocator.cpp"],
    hdrs = [
        "circular_allocator.h",
        "slot.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_test(
    name = "circular_allocator_test",
    srcs = [
        "circular_allocator_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":circular_allocator",
        "@googletest//:gtest_main",
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
        "@score_baselibs//score/mw/log:recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "dlt_argument_counter_test",
    srcs = [
        "dlt_argument_counter_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":dlt_argument_counter",
        "@googletest//:gtest_main",
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
    name = "empty_recorder_test",
    srcs = [
        "empty_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":empty_recorder",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "error_test",
    srcs = [
        "error_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":types_and_errors",
        "@googletest//:gtest_main",
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
        "helper_functions",
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
    name = "log_record_test",
    srcs = [
        "log_record_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":log_data_types",
        "@score_baselibs//score/quality/compiler_warnings",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "logging_identifier_test",
    srcs = [
        "logging_identifier_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":logging_identifier",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "empty_recorder_factory_test",
    srcs = [
        "empty_recorder_factory_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":empty_recorder_factory",
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
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/mw/log/configuration:target_config_reader_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "slot_test",
    srcs = [
        "slot_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":circular_allocator",
        "@googletest//:gtest_main",
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
        "@score_baselibs//score/mw/log:recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "verbose_payload_test",
    srcs = [
        "verbose_payload_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":log_data_types",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "recorder_factory_stub_test",
    srcs = ["recorder_factory_stub_test.cpp"],
    defines = ["KUSE_STUB_IMPLEMENTATION_ONLY"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "thread_local_guard_test",
    srcs = [
        "thread_local_guard_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":thread_local_guard",
        "@score_baselibs//score/concurrency:notification",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":circular_allocator_test",
        ":composite_recorder_test",
        ":dlt_argument_counter_test",
        ":dlt_format_test",
        ":empty_recorder_factory_test",
        ":thread_local_guard_test",
        ":empty_recorder_test",
        ":error_test",
        ":helper_functions_test",
        ":log_entry_deserialize_test",
        ":log_record_test",
        ":logging_identifier_test",
        ":recorder_factory_test",
        ":slot_test",
        ":statistics_reporter_test",
        ":verbose_payload_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/mw/log/detail/file_logging:unit_tests",
        #"@score_baselibs//score/mw/log/detail/data_router:unit_tests",
        "@score_baselibs//score/mw/log/detail/slog:unit_tests",
        #"@score_baselibs//score/mw/log/detail/wait_free_producer_queue:unit_tests",
        "@score_baselibs//score/mw/log/detail/wait_free_stack:unit_tests",
    ],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
    ],
)
