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

cc_library(
    name = "channel_mock",
    testonly = True,
    srcs = ["mock_channel.cpp"],
    hdrs = ["mock_channel.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:channel",
    ],
)

cc_library(
    name = "dispatch_mock",
    testonly = True,
    srcs = ["mock_dispatch.cpp"],
    hdrs = ["mock_dispatch.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:dispatch",
    ],
)

cc_library(
    name = "devctl_mock",
    testonly = True,
    srcs = ["mock_devctl.cpp"],
    hdrs = ["mock_devctl.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:devctl",
    ],
)

cc_library(
    name = "inout_mock",
    testonly = True,
    srcs = ["mock_inout.cpp"],
    hdrs = ["mock_inout.h"],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:inout",
    ],
)

cc_library(
    name = "mman_mock",
    testonly = True,
    srcs = ["mock_mman.cpp"],
    hdrs = ["mock_mman.h"],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:mman",
    ],
)

cc_library(
    name = "iofunc_mock",
    testonly = True,
    srcs = ["mock_iofunc.cpp"],
    hdrs = ["mock_iofunc.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:iofunc",
    ],
)

cc_library(
    name = "procmgr_mock",
    testonly = True,
    srcs = ["mock_procmgr.cpp"],
    hdrs = ["mock_procmgr.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:procmgr",
    ],
)

cc_library(
    name = "neutrino_qnx_mock",
    testonly = True,
    srcs = ["neutrino_qnx_mock.cpp"],
    hdrs = ["neutrino_qnx_mock.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:neutrino",
    ],
)

cc_library(
    name = "pci_qnx_mock",
    testonly = True,
    srcs = ["pci_qnx_mock.cpp"],
    hdrs = ["pci_qnx_mock.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:pci",
    ],
)

cc_library(
    name = "resmgr_mock",
    testonly = True,
    srcs = ["mock_resmgr.cpp"],
    hdrs = ["mock_resmgr.h"],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
        "@score_baselibs//score/os:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:resmgr",
    ],
)

cc_library(
    name = "secpol_mock",
    testonly = True,
    srcs = ["mock_secpol.cpp"],
    hdrs = ["mock_secpol.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:secpol",
    ],
)

cc_library(
    name = "secpolev_mock",
    testonly = True,
    srcs = ["mock_secpolev.cpp"],
    hdrs = ["mock_secpolev.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:secpolev",
    ],
)

cc_test(
    name = "mock_secpolev_test",
    size = "small",
    srcs = ["mock_secpolev_test.cpp"],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        ":secpolev_mock",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sysctl_mock",
    testonly = True,
    srcs = ["mock_sysctl.cpp"],
    hdrs = ["mock_sysctl.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:sysctl",
    ],
)

cc_library(
    name = "slog2_mock",
    testonly = True,
    srcs = ["mock_slog2.cpp"],
    hdrs = ["mock_slog2.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:slog2",
    ],
)

cc_library(
    name = "sysmgr_mock",
    testonly = True,
    srcs = ["mock_sysmgr.cpp"],
    hdrs = ["mock_sysmgr.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:sysmgr",
    ],
)

cc_library(
    name = "timer_mock",
    testonly = True,
    srcs = ["mock_timer.cpp"],
    hdrs = ["mock_timer.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:timer",
    ],
)

cc_library(
    name = "thread_ctl_mock",
    testonly = True,
    srcs = ["mock_thread_ctl.cpp"],
    hdrs = ["mock_thread_ctl.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:thread_ctl",
    ],
)

cc_library(
    name = "unistd_mock",
    testonly = True,
    srcs = ["mock_unistd.cpp"],
    hdrs = ["mock_unistd.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:unistd",
    ],
)

cc_library(
    name = "fs_crypto_mock",
    testonly = True,
    srcs = ["mock_fs_crypto.cpp"],
    hdrs = ["mock_fs_crypto.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:fs_crypto",
    ],
)

cc_library(
    name = "sigevent_qnx_mock",
    testonly = True,
    srcs = ["sigevent_qnx_mock.cpp"],
    hdrs = ["sigevent_qnx_mock.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/os/qnx:sigevent_qnx",
    ],
)
