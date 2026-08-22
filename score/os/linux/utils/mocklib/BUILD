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

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "network_interface_mock",
    testonly = True,
    srcs = ["network_interface_mock.cpp"],
    hdrs = ["network_interface_mock.h"],
    visibility = [
        "@score_baselibs//score/os/linux/utils/test:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os/linux/utils:network_interface",
        "@score_baselibs//score/os/mocklib:nonposixwrapper_mock",
    ],
)
