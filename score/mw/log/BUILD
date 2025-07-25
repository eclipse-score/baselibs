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

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "log",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":frontend",
    ] + select({
        "@score-baselibs//score/mw/log/detail/flags:config_KUse_Stub_Implementation_Only": [
            "@score-baselibs//score/mw/log/detail:recorder_factory_stub",
        ],
        "//conditions:default": [
            "@score-baselibs//score/mw/log/detail:recorder_factory",
        ],
    }),
)

cc_library(
    name = "recorder_interface",
    srcs = ["irecorder_factory.cpp"],
    hdrs = ["irecorder_factory.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/mw/log:__pkg__",
        "@score-baselibs//score/mw/log/detail:__subpackages__",
    ],
    deps = [":recorder"],
)

# TODO: make use of implementation_deps and private header files
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
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__subpackages__",
        "@score-baselibs//score/memory/shared:__subpackages__",  # Needed to break cyclic dependency (Ticket-105741)
        "@score-baselibs//score/mw/log/detail:__subpackages__",
        "@score-baselibs//score/mw/log/legacy_non_verbose_api:__pkg__",
        "@score-baselibs//score/mw/log/log_ext:__pkg__",
        "@score-baselibs//score/mw/log/test:__subpackages__",
        "@score-baselibs//score/mw/log/test/sctf_example:__subpackages__",
        "//platform/aas/sysfunc/common/logistics/error:__pkg__",
    ],
    deps = [
        ":log_stream",
        ":recorder",
        ":recorder_interface",
        ":shared_types",
        "@score-baselibs//score/result",
        "@score-baselibs//score/mw/log/detail:circular_allocator",
        "@score-baselibs//score/mw/log/detail:empty_recorder",
        "@score-baselibs//score/mw/log/detail:thread_local_guard",
        "@score-baselibs//score/mw/log/detail/wait_free_stack",
        "@score-baselibs//score/language/futurecpp",
    ],
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
        "//platform/aas/ara/log:__subpackages__",
        "@score-baselibs//score/mw/log/configuration:__pkg__",
        "@score-baselibs//score/mw/log/detail:__pkg__",
        "@score-baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score-baselibs//score/mw/log/detail/file_logging:__pkg__",
        "@score-baselibs//score/mw/log/detail/slog:__pkg__",
        "//platform/aas/test/pas/logging/utils:__pkg__",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "recorder",
    srcs = ["recorder.cpp"],
    hdrs = ["recorder.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/mw/log/detail:__pkg__",
        "@score-baselibs//score/mw/log/detail/data_router:__pkg__",
        "@score-baselibs//score/mw/log/detail/file_logging:__pkg__",
        "@score-baselibs//score/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":shared_types",
        "@score-baselibs//score/language/futurecpp",
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
    tags = ["FFI"],
    visibility = [
        "//platform/aas/lib/hash/code/common:__pkg__",
        "@score-baselibs//score/os:__pkg__",
        "@score-baselibs//score/mw/log/detail:__pkg__",
        "@score-baselibs//score/mw/log/log_ext:__pkg__",
        "@score-baselibs//score/mw/log/test/my_custom_lib:__pkg__",
        "@score-baselibs//score/mw/log/test/sample_ara_core_logging:__pkg__",
        "//platform/aas/sysfunc/BasicSecurity/code/secure_feature_activation:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/auto_sync/error:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/bank_information:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/common_programming_attributes/check_programming_dependencies_flag:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/common_programming_attributes/last_preprocessing_state_flag:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/data_provider/read_programming_counter/details:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/minion_data/error:__pkg__",
        "//platform/aas/sysfunc/SoftwareUpdate/code/mirror_protocol/01_preprocessing/preprocessing_step:__pkg__",
        "//platform/aas/sysfunc/common/logistics/error:__pkg__",
    ],
    deps = [
        ":recorder",
        ":shared_types",
        "@score-baselibs//score/result:error",
        "@score-baselibs//score/mw/log/detail:logging_identifier",
        "@score-baselibs//score/mw/log/detail:thread_local_guard",
        "@score-baselibs//score/language/futurecpp",
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
        ":log",
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
        ":log",
        ":recorder_mock",
        "@googletest//:gtest_main",
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
        ":log",
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
        ":log",
        ":recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "runtime_test",  # leave
    srcs = [
        ":runtime_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":frontend",
        ":recorder_mock",
        "@score-baselibs//score/mw/log/detail:recorder_factory",
        "@googletest//:gtest_main",
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
        ":log",
        ":recorder_mock",
        "@googletest//:gtest_main",
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
        ":log",
        ":recorder_mock",
        "@googletest//:gtest_main",
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
        "@score-baselibs//score/mw/log/detail:recorder_factory",
        "@score-baselibs//score/mw/log/test/my_custom_lib:my_custom_type_mw_log",
        "@googletest//:gtest_main",
    ],
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
        ":logger_container_test",
        ":logger_test",
        ":logging_test",
        ":runtime_test",
        ":runtime_test_instance_fallback",
        ":runtime_test_without_pointer_initialization",
        ":slot_handle_test",
    ],
    test_suites_from_sub_packages = [
        "@score-baselibs//score/mw/log/configuration:unit_tests",
        "@score-baselibs//score/mw/log/detail:unit_tests",
        #"@score-baselibs//score/mw/log/legacy_non_verbose_api:unit_tests",
        "@score-baselibs//score/mw/log/test:unit_test_suite",
    ],
    visibility = [
        "//platform/aas/mw:__pkg__",
    ],
)
