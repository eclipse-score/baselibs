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
    name = "network_interface",
    srcs = ["network_interface.cpp"],
    hdrs = ["network_interface.h"],
    defines = select({
        "//bazel/platforms:config_xpad_dnv": ["CORBOS_SECCOMP"],
        "//conditions:default": [],
    }),
    features = COMPILER_WARNING_FEATURES,
    linkopts = select({
        "//bazel/platforms:config_xpad_dnv": ["-lrt -lcap -lseccomp"],
        "//conditions:default": ["-lrt"],
    }),
    target_compatible_with = ["@platforms//os:linux"],
    visibility = [
        "//ecu/xpad/ara:__subpackages__",
        "@score_baselibs//score/os/linux/utils/mocklib:__subpackages__",
        "@score_baselibs//score/os/linux/utils/test:__subpackages__",
        "//platform/aas/pas/sysmon:__subpackages__",
    ],
    deps = select({
        "//bazel/platforms:config_xpad_dnv": [],
        "//conditions:default": [
            "//third_party/libseccomp2",
        ],
    }) + [
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/os/linux",
        "@score_baselibs//score/language/futurecpp",
    ],
)
