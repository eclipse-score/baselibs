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
load("@rules_rust//rust:defs.bzl", "rust_doc", "rust_library", "rust_test")
load("//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

rust_library(
    name = "libcpp",
    srcs = [
        "lib.rs",
        "string_view.rs",
    ],
    crate_features = select(
        {
            "@platforms//os:linux": ["libstdcpp_layout"],
            "@platforms//os:qnx": ["libcxx_layout"],
        },
        no_match_error = "Unsupported platform for libcpp crate",
    ),
    proc_macro_deps = [
        "@score_crates//:paste",
        "//score/language/rust/libcpp_derive",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_crates//:libc",
    ],
)

cc_library(
    name = "ffi_test_helpers",
    srcs = ["ffi_test_helpers.cpp"],
    hdrs = [],
    features = COMPILER_WARNING_FEATURES,
)

rust_doc(
    name = "doc",
    crate = ":libcpp",
    # Always use libstdcpp for docs as these aren't influenced by the layout
    crate_features = ["libstdcpp_layout"],
    tags = ["doc"],
)

rust_test(
    name = "unittest",
    crate = ":libcpp",
    crate_features = select({
        "@platforms//os:qnx": ["libcxx_layout"],
        "//conditions:default": ["libstdcpp_layout"],
    }),
    features = ["link_std_cpp_lib"],
    tags = ["unit"],
    deps = [":ffi_test_helpers"],
)

rust_test(
    name = "libcpp_test",
    srcs = ["libcpp_test.rs"],
    tags = ["unit"],
    deps = [":libcpp"],
)
