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

load("@cxx.rs//tools/bazel:rust_cxx_bridge.bzl", "rust_cxx_bridge")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "stop_token_adapter",
    srcs = ["cpp/stop_token_adapter.cpp"],
    hdrs = ["cpp/stop_token_adapter.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = ["@score_baselibs//score/language/futurecpp"],
)

rust_cxx_bridge(
    name = "stop_token_bridge",
    src = "src/lib.rs",
    deps = [":stop_token_adapter"],
)

rust_library(
    name = "stop_token",
    srcs = ["src/lib.rs"],
    visibility = ["//visibility:public"],
    deps = [
        ":stop_token_bridge",
        "@cxx.rs//:cxx",
    ],
)

rust_test(
    name = "stop_token_test",
    crate = ":stop_token",
    features = ["link_std_cpp_lib"],
    tags = ["unit"],
)
