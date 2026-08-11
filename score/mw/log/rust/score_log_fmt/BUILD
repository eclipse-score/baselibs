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
load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
    name = "score_log_fmt",
    srcs = glob(["**/*.rs"]),
    crate_features = select({
        "//score/mw/log/rust:safety_level_qm": ["qm"],
        "//score/mw/log/rust:safety_level_asil_b": [],
    }),
    visibility = [
        "//score/mw/log/rust/score_log:__pkg__",
        "//score/mw/log/rust/score_log_fmt_macro:__pkg__",
    ],
)

rust_test(
    name = "tests",
    crate = "score_log_fmt",
    tags = [
        "unit_tests",
        "ut",
    ],
)
