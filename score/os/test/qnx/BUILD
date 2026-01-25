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

load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

package(
    default_visibility = [
        "@score_baselibs//score/os/test/qnx:__subpackages__",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":channel_test",
        ":channel_impl_test",
        ":devctl_test",
        ":dispatch_test",
        ":dispatch_impl_test",
        ":iofunc_test",
        ":neutrino_test",
        ":pci_test",
        # TODO: To be fixed on Ticket-201117. Test not compatible with ARM64
        # ":pci_safety_test",
        ":procmgr_test",
        ":resmgr_test",
        ":secpol_test",
        ":slog2_test",
        ":sysctl_test",
        ":unistd_test",
        ":pthread_test",
        ":timer_test",
        ":thread_ctl_test",
        ":inout_test",
        ":mman_test",
        ":fs_crypto_test",
        ":fs_crypto_impl_test",
        ":pcap_test",
        ":sigevent_qnx_test",
        ":qnx_version_test",
    ],
    test_suites = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:unit_tests_qnx",
    ],
    visibility = ["//visibility:public"],  # platform_only
)

cc_test(
    name = "dispatch_test",
    srcs = ["dispatch_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:dispatch_mock",
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/qnx:procmgr",
        "@score_baselibs//score/os/qnx:timer",
    ],
)

cc_test(
    name = "pcap_test",
    srcs = ["pcap_test.cpp"],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = select({
        "@platforms//os:qnx": [
            "@score_bazel_platforms//:qnx7_1",  # TODO: locally For QNX8 Filter needs to be expliclity added otherwise no packets are recived, on CI this is not working at all even with this filtering it needs to be investigated why in Ticket-221732
        ],
        "//conditions:default": [
            "@platforms//os:qnx",
        ],
    }),
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:pcap",
    ],
)

cc_test(
    name = "dispatch_impl_test",
    srcs = ["dispatch_impl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/qnx:procmgr",
    ],
)

cc_test(
    name = "channel_test",
    srcs = ["channel_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:channel_mock",
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
    ],
)

cc_test(
    name = "channel_impl_test",
    srcs = ["channel_impl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
    ],
)

cc_test(
    name = "devctl_test",
    srcs = ["devctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os/mocklib/qnx:devctl_mock",
        "@score_baselibs//score/os/qnx:devctl",
    ],
)

cc_test(
    name = "iofunc_test",
    srcs = ["iofunc_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:iofunc_mock",
        "@score_baselibs//score/os/qnx:iofunc",
    ],
)

cc_test(
    name = "neutrino_test",
    srcs = [
        "neutrino_impl_test.cpp",
        "neutrino_showcase_test.cpp",
        "neutrino_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/os/mocklib/qnx:neutrino_qnx_mock",
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/qnx:neutrino",
        "@score_baselibs//score/os/qnx:sigevent_qnx",
    ],
)

cc_test(
    name = "procmgr_test",
    srcs = ["procmgr_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:procmgr_mock",
        "@score_baselibs//score/os/qnx:procmgr",
    ],
)

cc_test(
    name = "resmgr_test",
    srcs = [
        "resmgr_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:resmgr",
    ],
)

cc_test(
    name = "secpol_test",
    srcs = [
        "secpol_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:secpol",
    ],
)

cc_test(
    name = "slog2_test",
    srcs = [
        "slog2_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:slog2",
    ],
)

cc_test(
    name = "pci_test",
    srcs = [
        "pci_test.cpp",
    ],
    tags = [
        "manual",
        "unit",
    ],
    #TODO: Ticket-216047: Fix disabled tests which failed on qnx8
    target_compatible_with = [
        "@platforms//os:qnx",
        "@score_bazel_platforms//:qnx7_1",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:neutrino",
        "@score_baselibs//score/os/qnx:pci",
    ],
)

cc_test(
    name = "pci_safety_test",
    srcs = [
        "pci_safety_test.cpp",
    ],
    tags = ["manual"],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:pci_safety",
    ],
)

cc_test(
    name = "sysctl_test",
    srcs = ["sysctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    #TODO: Ticket-215992: Fix disabled tests which failed on qnx8
    target_compatible_with = [
        "@platforms//os:qnx",
        "@score_bazel_platforms//:qnx7_1",
    ],
    deps = [
        "//third_party/libsocket",
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:sysctl_mock",
        "@score_baselibs//score/os/qnx:sysctl",
    ],
)

cc_test(
    name = "unistd_test",
    srcs = ["unistd_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/os/qnx:unistd",
    ],
)

cc_test(
    name = "pthread_test",
    srcs = [
        "pthread_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os:pthread",
    ],
)

cc_test(
    name = "timer_test",
    srcs = ["timer_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:timer_mock",
        "@score_baselibs//score/os/qnx:timer",
    ],
)

cc_test(
    name = "thread_ctl_test",
    srcs = ["thread_ctl_test.cpp"],
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:thread_ctl_mock",
        "@score_baselibs//score/os/qnx:thread_ctl",
    ],
)

cc_test(
    name = "inout_test",
    srcs = ["inout_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    #TODO: Ticket-215992: Fix disabled tests which failed on qnx8
    target_compatible_with = [
        "@platforms//os:qnx",
        "@score_bazel_platforms//:qnx7_1",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:inout",
    ],
)

cc_test(
    name = "mman_test",
    srcs = ["mman_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:mman",
    ],
)

cc_test(
    name = "fs_crypto_test",
    srcs = ["fs_crypto_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/mocklib/qnx:fs_crypto_mock",
        "@score_baselibs//score/os/qnx:fs_crypto",
    ],
)

cc_test(
    name = "fs_crypto_impl_test",
    srcs = ["fs_crypto_impl_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:fs_crypto",
    ],
)

cc_test(
    name = "sigevent_qnx_test",
    srcs = ["sigevent_qnx_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/os/qnx:sigevent_qnx",
    ],
)

cc_test(
    name = "qnx_version_test",
    srcs = [
        "qnx_version_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/qnx:qnx_version",
    ],
)
