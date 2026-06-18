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
load("@rules_rust//rust:defs.bzl", "rust_static_library")

rust_static_library(
    name = "ffi",
    srcs = ["ffi.rs"],
    edition = "2021",
    visibility = ["//visibility:private"],
    deps = [
        "//src/log/score_log",
        "//src/log/stdout_logger",
    ],
)

cc_library(
    name = "stdout_logger_cpp_init",
    srcs = ["stdout_logger_init.cpp"],
    hdrs = ["stdout_logger_init.h"],
    include_prefix = "score/mw/log/rust",
    visibility = ["//visibility:public"],
    deps = [
        ":ffi",
    ],
)
