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
load("@rules_rust//rust:defs.bzl", "rust_proc_macro")

rust_proc_macro(
    name = "score_testing_macros",
    srcs = glob(["*.rs"]),
    visibility = ["//visibility:public"],
    deps = [
        "//src/log/stdout_logger",
        "@score_crates//:quote",
        "@score_crates//:syn",
    ],
)
