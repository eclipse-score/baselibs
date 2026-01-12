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

exports_files([
    "capability.h",
    "capability.cpp",
])

cc_library(
    name = "linux_header",
    hdrs = [
        "endmntent.h",
        "getmntent.h",
        "ifaddrs.h",
        "linuxsched.h",
        "pthread.h",
        "setmntent.h",
        "shm.h",
        "statfs.h",
        "sysinfo.h",
        "timerfd.h",
        "timex.h",
        "unistd.h",
    ],
    defines = ["ALLOW_QM"],
    target_compatible_with = ["@platforms//os:linux"],
    visibility = [
        "@score_baselibs//score/os:__subpackages__",
        "@score_baselibs//score/os/mocklib/linux:__subpackages__",
    ],
)

cc_library(
    name = "linux",
    srcs = [
        "endmntent.cpp",
        "getmntent.cpp",
        "ifaddrs.cpp",
        "linuxsched.cpp",
        "pthread.cpp",
        "setmntent.cpp",
        "shm.cpp",
        "statfs.cpp",
        "sysinfo.cpp",
        "timerfd.cpp",
        "timex.cpp",
        "unistd.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    linkopts = select({
        "//bazel/platforms:config_xpad_dnv": [
            "-lcap",
        ],
        "//conditions:default": [],
    }) + [
        "-lrt",
        "-lpthread",
    ],
    target_compatible_with = ["@platforms//os:linux"],
    visibility = [
        "//ecu/xpad/xpad-shared/config/mpad/pp/taf_plus/pas/logging/test:__pkg__",
        "//platform/aas/ara/tsync:__subpackages__",
        "//platform/aas/pas/crash_handler:__pkg__",
        "//platform/aas/pas/sysmon:__pkg__",
        "//platform/aas/pas/sysmon/application:__subpackages__",
        "@score_baselibs//score/os:__subpackages__",
    ],
    deps = select({
        "//bazel/platforms:config_xpad_dnv": [],
        "//conditions:default": [
            "//third_party/libcap2",
        ],
    }) + [
        ":linux_header",
    ],
)
