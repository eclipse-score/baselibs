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

alias(
    name = "log",
    actual = ":frontend",
    deprecation = "DEPRECATED: use @score_baselibs//score/mw/log:frontend instead. Refer usage in mw/log/README.md",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "frontend",
    srcs = [
        "log_stream_factory.cpp",
        "logger.cpp",
        "logger_container.cpp",
        "logging.cpp",
        "runtime.cpp",
    ],
    hdrs = [
        "log_stream_factory.h",
        "logger.h",
        "logger_container.h",
        "logging.h",
        "runtime.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        ":recorder",
        ":recorder_interface",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail:circular_allocator",
        "@score_baselibs//score/mw/log/detail:empty_recorder",
        "@score_baselibs//score/mw/log/detail:thread_local_guard",
        "@score_baselibs//score/result",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":log_stream",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",  #Ticket-222244
    ],
)

alias(
    name = "backend_stub_testutil",
    testonly = True,
    actual = "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "recorder_interface",
    srcs = ["irecorder_factory.cpp"],
    hdrs = ["irecorder_factory.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [":recorder"],
)

cc_library(
    name = "shared_types",
    srcs = [
        "log_common.cpp",
        "log_level.cpp",
        "slot_handle.cpp",
    ],
    hdrs = [
        "log_common.h",
        "log_level.h",
        "log_mode.h",
        "log_types.h",
        "slot_handle.h",
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
    name = "recorder",
    srcs = ["recorder.cpp"],
    hdrs = ["recorder.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":shared_types",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "recorder_mock",
    testonly = True,
    hdrs = ["recorder_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":recorder",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "log_stream",
    srcs = [
        "log_stream.cpp",
        "log_stream_error.cpp",
    ],
    hdrs = ["log_stream.h"],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail:thread_local_guard",
        "@score_baselibs//score/result:error",
    ],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":recorder",
        ":shared_types",
        "@score_baselibs//score/mw/log/detail:logging_identifier",
    ],
)

cc_test(
    name = "log_level_test",
    srcs = [
        ":log_level_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":shared_types",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "log_stream_error_test",
    srcs = [
        ":log_stream_error_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "logger_container_test",
    srcs = [
        ":logger_container_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",
    ],
)

cc_test(
    name = "logger_test",
    srcs = [
        ":logger_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "logging_test",
    srcs = [
        ":logging_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",
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
        "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
    ],
)

cc_test(
    name = "runtime_test_instance_fallback",
    srcs = [
        "runtime_test_instance_fallback.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",
    ],
)

cc_test(
    name = "runtime_test_without_pointer_initialization",
    srcs = [
        "runtime_test_without_pointer_initialization.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_stub_testutil",
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",
    ],
)

cc_test(
    name = "slot_handle_test",
    srcs = [
        ":slot_handle_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":shared_types",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "log_stream_test",
    srcs = [
        ":log_stream_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
        "@score_baselibs//score/mw/log/test/my_custom_lib:my_custom_type_mw_log",
    ],
)

# Allows unit testing with production code running with the recorder_factory
# that includes all the backends supporting coverage and qualification.
exports_files(
    ["log_stream_test.cpp"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "log_stream_factory_test",
    srcs = [
        "log_stream_factory_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":frontend",
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:recorder_factory_stub",
        "@score_baselibs//score/mw/log/detail:thread_local_guard",
    ],
)

# Allows unit testing with production code running with the recorder_factory
# that includes all the backends supporting coverage and qualification.
exports_files(
    ["log_stream_factory_test.cpp"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "log_types_test",
    srcs = [
        ":log_types_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":shared_types",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":log_stream_test",
        ":log_types_test",
        ":log_level_test",
        ":log_stream_error_test",
        ":log_stream_factory_test",
        ":logger_container_test",
        ":logger_test",
        ":logging_test",
        ":runtime_test",
        ":runtime_test_instance_fallback",
        ":runtime_test_without_pointer_initialization",
        ":slot_handle_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/mw/log/configuration:unit_tests",
        "@score_baselibs//score/mw/log/detail:unit_tests",
        #"@score_baselibs//score/mw/log/legacy_non_verbose_api:unit_tests",
        #"@score_baselibs//score/mw/log/test:unit_test_suite",
    ],
    visibility = [
        "//platform/aas/mw:__pkg__",
    ],
)
