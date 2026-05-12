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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "trace_library_mock",
    srcs = ["trace_library_mock.cpp"],
    hdrs = ["trace_library_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/ara:__subpackages__",  # TODO Ticket-259481
        "//platform/aas/mw/com:__subpackages__",  # To be removed once me/com is pulled via bzlmod
        "@score_baselibs//score/analysis/tracing/generic_trace_library:__pkg__",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/test:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:trace_library_interface",
    ],
)
