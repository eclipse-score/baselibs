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

load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

rust_library(
    name = "pal",
    srcs = glob(["**/*.rs"]),
    edition = "2021",
    visibility = ["//src:__subpackages__"],
    deps = [
        "//src/containers",
        "//src/log/score_log",
        "@score_crates//:libc",
    ],
)

rust_test(
    name = "tests",
    crate = "pal",
    tags = [
        "unit_tests",
        "ut",
    ],
)
