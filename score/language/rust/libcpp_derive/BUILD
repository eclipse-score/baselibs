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
load("@rules_rust//rust:defs.bzl", "rust_doc_test", "rust_proc_macro", "rust_test")

rust_proc_macro(
    name = "libcpp_derive",
    srcs = ["lib.rs"],
    visibility = [
        "//score/language/rust/libcpp:__pkg__",
    ],
    deps = [
        "@score_crates//:proc_macro2",
        "@score_crates//:quote",
        "@score_crates//:syn",
    ],
)

rust_doc_test(
    name = "doc_test",
    crate = ":libcpp_derive",
    tags = ["unit"],
)

cc_library(
    name = "unit_test_helpers",
    srcs = ["unit_test_helpers.cpp"],
)

rust_test(
    name = "unit_test",
    srcs = ["unit_test.rs"],
    features = ["link_std_cpp_lib"],
    proc_macro_deps = [":libcpp_derive"],
    tags = ["unit"],
    deps = [
        ":unit_test_helpers",
        "@score_crates//:cxx",
    ],
)
