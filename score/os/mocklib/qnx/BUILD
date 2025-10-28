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
        "@score_baselibs//score/os/qnx:channel",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:dispatch",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:devctl",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:inout",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:mman",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:iofunc",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:procmgr",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:neutrino",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:pci",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/os/qnx:resmgr",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:secpol",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:secpolev",
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
        "@score_baselibs//score/os/qnx:sysctl",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:slog2",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:sysmgr",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:timer",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:thread_ctl",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:unistd",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:fs_crypto",
        "@googletest//:gtest",
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
        "@score_baselibs//score/os/qnx:sigevent_qnx",
        "@googletest//:gtest",
    ],
)
