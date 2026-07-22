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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_crates//third_party/cxx:rust_cxx_bridge.bzl", "rust_cxx_bridge")

cc_library(
    name = "ffi_test_helpers",
    testonly = True,
    srcs = ["ffi_test_helpers.cpp"],
    hdrs = [],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        "@score_baselibs//score/result:error",
        "@score_baselibs//score/result:error_msg_mapping",
        "@score_baselibs//score/result:result_for_rust",
    ],
)

rust_library(
    name = "result_rs",
    srcs = ["result.rs"],
    crate_features = select(
        {
            "@platforms//os:linux": ["libstdcpp_layout"],
            "@platforms//os:qnx": ["libcxx_layout"],
        },
        no_match_error = "Unsupported platform for result_rs crate",
    ),
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/language/rust/libcpp",
        "@score_baselibs//score/result:error_msg_mapping",
        "@score_crates//:libc",
    ],
)

# Add this as a convenience alias to allow for dependency declarations of the form @score_baselibs//score/result/rust
alias(
    name = "rust",
    actual = ":result_rs",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "result_cxx",
    hdrs = ["result_cxx.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/result:result_for_rust",
        "@score_crates//:cxx_cc",
    ],
)

rust_doc(
    name = "result_rs_doc",
    crate = ":result_rs",
    # Always use libstdcpp for docs as these aren't influenced by the layout
    crate_features = ["libstdcpp_layout"],
    tags = ["doc"],
)

rust_test(
    name = "result_rs_test",
    crate = ":result_rs",
    crate_features = select({
        "@platforms//os:qnx": ["libcxx_layout"],
        "//conditions:default": ["libstdcpp_layout"],
    }),
    features = ["link_std_cpp_lib"],
    tags = ["unit"],
    deps = [":ffi_test_helpers"],
)

cc_library(
    name = "result_example_cpp",
    srcs = ["result_example_cpp.cpp"],
    hdrs = ["result_example_cpp.h"],
    deps = [
        ":result_cxx",
        "@score_baselibs//score/result:result_for_rust",
        "@score_crates//:cxx_cc",
    ],
)

rust_cxx_bridge(
    name = "result_example_bridge",
    src = "result_example.rs",
    deps = [
        ":result_example_cpp",
    ],
)

rust_test(
    name = "result_rs_example",
    srcs = ["result_example.rs"],
    features = ["link_std_cpp_lib"],
    tags = ["unit"],
    deps = [
        ":result_example_bridge",
        ":result_example_cpp",
        ":result_rs",
        "@score_crates//:cxx",
    ],
)
