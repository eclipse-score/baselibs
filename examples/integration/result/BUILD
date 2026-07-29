# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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

# Demo test: demonstrates defining a custom error domain, returning score::Result<T>
# from functions, and handling both the success and error paths.
cc_test(
    name = "result_demo",
    size = "small",
    srcs = ["result_demo_test.cpp"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/result",
    ],
)
