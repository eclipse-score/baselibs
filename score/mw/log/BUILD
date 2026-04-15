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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
        "spp_code_style_check_method_names",
        "spp_code_style_check_readability",
        "spp_code_style_check_type_names",
        "spp_code_style_check_variable_names",
    ],
)

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

alias(
    name = "log",
    actual = ":console",
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
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log/detail:circular_allocator",
        "@score_baselibs//score/mw/log/detail:empty_recorder",
        "@score_baselibs//score/mw/log/detail:recorder_interface",
        "@score_baselibs//score/mw/log/detail:thread_local_guard",
        "@score_baselibs//score/result",
        "@score_baselibs//score/utils/meyer_singleton",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":log_stream",
        "@score_baselibs//score/mw/log/detail/wait_free_stack",  #Ticket-222244
    ],
)

# frontend + empty backend, supports additive backend registration
cc_library(
    name = "minimal",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/mw/log:__subpackages__"],
    deps = [
        ":frontend",
        "@score_baselibs//score/mw/log:backend_table",
        "@score_baselibs//score/mw/log/detail:registry_aware_recorder_factory",
    ],
    alwayslink = True,
)

# frontend + console backend, supports additive backend registration
cc_library(
    name = "console",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":frontend",
        "@score_baselibs//score/mw/log/detail:backend_console",
    ],
)

# backend registration API for backend registrants
cc_library(
    name = "backend_table",
    srcs = ["backend_table.cpp"],
    hdrs = ["backend_table.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/mw/log:__subpackages__"],
    deps = [
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
    ],
)

alias(
    name = "console_only_backend",
    actual = ":console",
    deprecation = "Deprecated: Please use @score_baselibs//score/mw/log:console instead",
    visibility = ["//visibility:public"],
)

alias(
    name = "backend_stub_testutil",
    testonly = True,
    actual = ":console",  # Supports backward compatibility
    deprecation = "Deprecated: Please use @score_baselibs//score/mw/log:console OR @score_baselibs//score/mw/log:minimal instead",
    visibility = ["//visibility:public"],
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
        ":minimal",
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
        ":minimal",
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
        ":minimal",
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
        ":minimal",
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
    local_defines = select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": ["KCONSOLE_LOGGING"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:console",
        "@score_baselibs//score/mw/log:recorder_mock",
    ] + select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": [
            "@score_baselibs//score/mw/log/detail/text_recorder",
        ],
        "//conditions:default": [],
    }),
)

cc_test(
    name = "runtime_test_instance_fallback",
    srcs = [
        "runtime_test_instance_fallback.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    local_defines = select({
        "@score_baselibs//score/mw/log/detail/flags:config_KConsole_Logging": ["KCONSOLE_LOGGING"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        ":recorder_mock",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:console",
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
        ":minimal",
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
        ":minimal",
        ":recorder_mock",
        "@googletest//:gtest_main",
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
        ":minimal",
        ":recorder_mock",
        "@googletest//:gtest_main",
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

cc_test(
    name = "backend_table_test",
    srcs = ["backend_table_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_table",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:empty_recorder",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":backend_table_test",
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
        "@score_baselibs//score/mw/log/backend:unit_tests",
        "@score_baselibs//score/mw/log/configuration:unit_tests",
        "@score_baselibs//score/mw/log/detail:unit_tests",
        #"@score_baselibs//score/mw/log/legacy_non_verbose_api:unit_tests",
        #"@score_baselibs//score/mw/log/test:unit_test_suite",
    ],
    visibility = [
        "//platform/aas/mw:__pkg__",
    ],
)
