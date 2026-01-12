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
    name = "log_recorder_factory",
    srcs = [
        "log_recorder_factory.hpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/os:fcntl",
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
        "//visibility:public",
    ],
    deps = [
        ":empty_recorder",
        ":initialization_reporter",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
    ],
)

cc_library(
    name = "backend_interface",
    srcs = ["backend.cpp"],
    hdrs = ["backend.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
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
        "//visibility:public",
    ],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/mw/log/detail:backend_interface",
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
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
    ],
)

cc_library(
    name = "types_and_errors",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "log_entry",
    srcs = ["log_entry.cpp"],
    hdrs = ["log_entry.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":logging_identifier",
        "@score_baselibs//score/mw/log:shared_types",
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
        "//visibility:public",
    ],
    deps = [
        ":helper_functions",
        ":log_entry",
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
        "//visibility:public",
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
        "//visibility:public",
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
        "//visibility:public",
    ],
    deps = [
        ":types_and_errors",
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
        "//visibility:public",
    ],
    deps = [
        ":log_data_types",
        ":log_entry",
        ":logging_identifier",
        ":types_and_errors",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:recorder",
    ],
)

cc_library(
    name = "thread_local_guard",
    srcs = ["thread_local_guard.cpp"],
    hdrs = ["thread_local_guard.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
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
        "@score_baselibs//score/mw/log/detail/text_recorder",
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
        "//visibility:public",
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
        ":helper_functions",
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
        "@googletest//:gtest_main",
        "@score_baselibs//score/quality/compiler_warnings",
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
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
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
        "@googletest//:gtest_main",
        "@score_baselibs//score/concurrency:notification",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":circular_allocator_test",
        ":dlt_argument_counter_test",
        ":empty_recorder_factory_test",
        ":thread_local_guard_test",
        ":recorder_factory_stub_test",
        ":empty_recorder_test",
        ":error_test",
        ":helper_functions_test",
        ":log_record_test",
        ":logging_identifier_test",
        ":slot_test",
        ":verbose_payload_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/mw/log/detail/text_recorder:unit_tests",
        #"@score_baselibs//score/mw/log/detail/data_router:unit_tests",
        #"@score_baselibs//score/mw/log/detail/slog:unit_tests",
        #"@score_baselibs//score/mw/log/detail/wait_free_producer_queue:unit_tests",
        "@score_baselibs//score/mw/log/detail/wait_free_stack:unit_tests",
        #"@score_baselibs//score/mw/log/detail/common:unit_tests",
        #"@score_baselibs//score/mw/log/detail/file_recorder:unit_tests",
    ],
    visibility = [
        "@score_baselibs//score/mw/log:__pkg__",
    ],
)
