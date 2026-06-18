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
load("@rules_rust//rust:defs.bzl", "rust_proc_macro", "rust_test")

rust_proc_macro(
    name = "score_log_fmt_macro",
    srcs = glob(["*.rs"]),
    visibility = ["//src/log/score_log:__pkg__"],
    deps = [
        "//src/log/score_log_fmt",
        "@score_crates//:proc_macro2",
        "@score_crates//:quote",
        "@score_crates//:syn",
    ],
)

rust_test(
    name = "tests",
    srcs = glob(["tests/**/*.rs"]),
    proc_macro_deps = [
        "//src/log/score_log_fmt_macro",
    ],
    tags = [
        "unit_tests",
        "ut",
    ],
    deps = [
        "//src/log/score_log",
        "//src/log/score_log_fmt",
    ],
)
