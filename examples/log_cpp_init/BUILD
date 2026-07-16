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

load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_rust//rust:defs.bzl", "rust_static_library")

rust_static_library(
    name = "example_lib",
    srcs = ["src/example_lib.rs"],
    edition = "2021",
    visibility = ["//visibility:private"],
    deps = [
        "//score/mw/log/rust/score_log",
        "//score/mw/log/rust/stdout_logger",
    ],
)

cc_binary(
    name = "log_cpp_init",
    srcs = ["src/main.cpp"],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
    deps = [
        ":example_lib",
        "//score/mw/log/rust/stdout_logger_cpp_init",
    ],
)
