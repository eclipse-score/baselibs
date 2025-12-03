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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":abortable_blocking_reader_test",
        ":detect_os_test",
        ":high_resolution_steady_clock_test",
        ":machine_test",
        ":mqueue_integration_test",
        ":mqueue_unit_test",
        ":path_test",
        ":pmutex_test",
        ":signal_test",
        ":spinlock_test",
        ":tcp_keep_alive_test",
        ":thread_test",
        "@score_baselibs//score/os/utils/acl:unit_test",
        "@score_baselibs//score/os/utils/inotify:unit_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/os/utils/interprocess:unit_tests",
    ],
    visibility = ["//platform/aas/lib:__pkg__"],
)

cc_test(
    name = "abortable_blocking_reader_test",
    srcs = [
        "abortable_blocking_reader_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:sys_poll",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:sys_poll_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@score_baselibs//score/os/utils:abortable_blocking_reader",
    ],
)

cc_test(
    name = "tcp_keep_alive_test",
    srcs = [
        "tcp_keep_alive_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/utils:tcp_keep_alive",
    ],
)

cc_test(
    name = "mqueue_integration_test",
    srcs = [
        "mqueueintegration_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/utils:mqueue",
    ],
)

cc_test(
    name = "mqueue_unit_test",
    srcs = [
        "mqunit_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib:mqueue_mock",
        "@score_baselibs//score/os/mocklib:stat_mock",
        "@score_baselibs//score/os/utils:mqueue",
    ],
)

cc_test(
    name = "signal_test",
    srcs = ["signal_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/os/utils:signal",
    ],
)

cc_test(
    name = "thread_test",
    srcs = ["thread_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/os/utils:thread",
    ],
)

cc_test(
    name = "pmutex_test",
    srcs = ["pmutexTest.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/utils/interprocess:interprocess_mutex",
    ],
)

cc_test(
    name = "high_resolution_steady_clock_test",
    srcs = ["high_resolution_steady_clock_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/utils:high_resolution_steady_clock",
    ],
)

cc_test(
    name = "spinlock_test",
    srcs = ["spinlocktest.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/utils:spinlock",
    ],
)

cc_test(
    name = "path_test",
    srcs = ["path_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "unit",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@score_baselibs//score/os/utils:path",
    ],
)

cc_test(
    name = "detect_os_test",
    srcs = ["detect_os_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "unit",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib:uname_mock",
        "@score_baselibs//score/os/utils:detect_os",
    ],
)

cc_test(
    name = "machine_test",
    srcs = ["machine_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    local_defines = select({
        "@score_baselibs//bazel/settings/qemu:on_qemu": ["MACHINE_QEMU"],
        "//conditions:default": [],
    }),
    tags = [
        "unit",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/utils:machine",
    ],
)
