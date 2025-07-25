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
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

exports_files(["capability.h"])

cc_library(
    name = "channel",
    srcs = [
        "channel.cpp",
        "channel_impl.cpp",
    ],
    hdrs = [
        "channel.h",
        "channel_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "dispatch",
    srcs = [
        "dispatch.cpp",
        "dispatch_impl.cpp",
    ],
    hdrs = [
        "dispatch.h",
        "dispatch_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "devctl",
    srcs = [
        "devctl.cpp",
        "devctl_impl.cpp",
    ],
    hdrs = [
        "devctl.h",
        "devctl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "inout",
    srcs = [
        "inout.cpp",
        "inout_impl.cpp",
    ],
    hdrs = [
        "inout.h",
        "inout_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "mman",
    srcs = [
        "mman.cpp",
        "mman_impl.cpp",
    ],
    hdrs = [
        "mman.h",
        "mman_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "iofunc",
    srcs = [
        "iofunc.cpp",
    ],
    hdrs = [
        "iofunc.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "pcap",
    srcs = [
        "pcap.cpp",
    ],
    hdrs = [
        "pcap.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/os:__subpackages__",
        "//platform/aas/network/xnm:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/libpcap",
    ],
)

cc_library(
    name = "procmgr",
    srcs = ["procmgr.cpp"],
    hdrs = ["procmgr.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "neutrino",
    srcs = [
        "neutrino.cpp",
        "neutrino_impl.cpp",
    ],
    hdrs = [
        "neutrino.h",
        "neutrino_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/bitmanipulation:bitmask_operators",
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "@score-baselibs//score/os:sigevent",
    ],
)

cc_library(
    name = "pci",
    srcs = [
        "pci.cpp",
        "pci_impl.cpp",
    ],
    hdrs = [
        "pci.h",
        "pci_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis/tracing/daemon:__subpackages__",
        "@score-baselibs//score/os/mocklib/qnx:__subpackages__",
        "@score-baselibs//score/os/test/qnx:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/libpci",
    ],
)

cc_library(
    name = "pci_safety",
    srcs = [
        "pci_safety.cpp",
        "pci_safety_impl.cpp",
    ],
    hdrs = [
        "pci_safety.h",
        "pci_safety_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/os:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/libpci_safety",
    ],
)

cc_library(
    name = "resmgr",
    srcs = [
        "resmgr.cpp",
        "resmgr_impl.cpp",
    ],
    hdrs = [
        "resmgr.h",
        "resmgr_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/os:__subpackages__",
        "//platform/aas/pas/hw_reg_res_mgrs:__subpackages__",
    ],
    deps = [
        ":types",
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "timer",
    srcs = [
        "timer.cpp",
        "timer_impl.cpp",
    ],
    hdrs = [
        "timer.h",
        "timer_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "types",
    srcs = [
        "types/i_open_function.cpp",
        "types/i_read_function.cpp",
        "types/i_seek_function.cpp",
        "types/i_write_function.cpp",
    ],
    hdrs = [
        "types/i_open_function.h",
        "types/i_read_function.h",
        "types/i_seek_function.h",
        "types/i_write_function.h",
        "types/iofunc_types.h",
        "types/types.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
    ],
)

cc_library(
    name = "secpol",
    srcs = [
        "secpol.cpp",
        "secpol_impl.cpp",
    ],
    hdrs = [
        "secpol.h",
        "secpol_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/libsecpol",
    ],
)

cc_library(
    name = "sysctl",
    srcs = [
        "sysctl.cpp",
        "sysctl_impl.cpp",
    ],
    hdrs = [
        "sysctl.h",
        "sysctl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "slog2_impl",
    srcs = [
        "slog2.cpp",
        "slog2_impl.cpp",
    ],
    hdrs = [
        "slog2.h",
        "slog2_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/os/qnx:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/libslog2",
    ],
)

alias(
    name = "slog2",
    actual = "slog2_impl",
    visibility = [
        "//visibility:public", # platform_only
    ],
)

cc_library(
    name = "sysmgr",
    srcs = [
        "sysmgr.cpp",
        "sysmgr_impl.cpp",
    ],
    hdrs = [
        "sysmgr.h",
        "sysmgr_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
    ],
)

cc_library(
    name = "secpolev",
    srcs = [
        "secpolev.cpp",
    ],
    hdrs = [
        "secpolev.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/os/mocklib/qnx:__pkg__",
        "//platform/aas/os/secpol/secpollog:__pkg__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:object_seam",
        "//third_party/qnx:libsecpolev",
    ],
)

cc_library(
    name = "thread_ctl",
    srcs = [
        "thread_ctl.cpp",
        "thread_ctl_impl.cpp",
    ],
    hdrs = [
        "thread_ctl.h",
        "thread_ctl_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "unistd",
    srcs = [
        "unistd.cpp",
        "unistd_impl.cpp",
    ],
    hdrs = [
        "unistd.h",
        "unistd_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score-baselibs//score/os:errno",
    ],
)

cc_library(
    name = "fs_crypto",
    srcs = [
        "fs_crypto.cpp",
        "fs_crypto_impl.cpp",
    ],
    hdrs = [
        "fs_crypto.h",
        "fs_crypto_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//platform/aas/intc/configure_persistency:__subpackages__",
        "@score-baselibs//score/os:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:errno",
        "//third_party/fscrypto",
    ],
)

cc_library(
    name = "sigevent_qnx",
    srcs = [
        "sigevent_qnx.cpp",
        "sigevent_qnx_impl.cpp",
    ],
    hdrs = [
        "sigevent_qnx.h",
        "sigevent_qnx_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score-baselibs//score/analysis/tracing:__subpackages__",
        "@score-baselibs//score/os:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/os:sigevent",
    ],
)
