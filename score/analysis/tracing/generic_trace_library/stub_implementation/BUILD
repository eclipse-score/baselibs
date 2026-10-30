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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "stub_implementation",
    srcs = [
        "generic_trace_api_stub.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//score/analysis/tracing/generic_trace_library:__pkg__",
        "//score/analysis/tracing/generic_trace_library/test:__subpackages__",
    ],
    deps = [
        "//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api",
        "//score/analysis/tracing/generic_trace_library/interface_types:trace_library_interface",
        "//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "generic_trace_api_stub_test",
    srcs = [
        "generic_trace_api_stub_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":stub_implementation",
        "//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api",
    ],
)
