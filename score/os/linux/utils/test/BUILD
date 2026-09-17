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

load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "network_interface_test",
    srcs = [
        "network_interface_test.cpp",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/os/linux/utils/mocklib:network_interface_mock",
        "@score_baselibs//score/os/mocklib:nonposixwrapper_mock",
        "@score_baselibs//score/os/mocklib:stat_mock",
    ] + ["@score_baselibs//score/os/linux/utils:network_interface"],
)
