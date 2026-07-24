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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_rust//rust:defs.bzl", "rust_binary")
load("//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "example_helpers",
    srcs = ["example_helpers.cpp"],
    hdrs = ["example_helpers.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//score/language/rust/memres:__pkg__"],
    deps = ["//score/language/futurecpp"],
)

# Demonstrates MemoryResource in hermetic monotonic mode.
# Running this binary intentionally terminates the process via std::abort()
# to show the hermetic boundary in action.
rust_binary(
    name = "memres_example",
    srcs = ["main.rs"],
    crate_root = "main.rs",
    edition = "2021",
    features = ["link_std_cpp_lib"],
    deps = [
        ":example_helpers",
        "//score/language/rust/memres",
    ],
)
