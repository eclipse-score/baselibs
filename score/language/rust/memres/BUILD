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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "guarded_upstream",
    srcs = ["cpp/guarded_upstream.cpp"],
    hdrs = ["cpp/guarded_upstream.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        "//score/language/futurecpp",
        "//score/mw/log",
        "//score/mw/log:frontend",
    ],
)

cc_library(
    name = "memory_resource_handle",
    srcs = ["cpp/memory_resource_handle.cpp"],
    hdrs = ["cpp/memory_resource_handle.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":guarded_upstream"],
)

cc_library(
    name = "memres_cpp",
    srcs = ["cpp/ffi.cpp"],
    hdrs = ["cpp/ffi.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":memory_resource_handle"],
)

rust_library(
    name = "memres",
    srcs = ["src/lib.rs"],
    crate_root = "src/lib.rs",
    edition = "2021",
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [":memres_cpp"],
)

cc_test(
    name = "memres_test",
    srcs = ["test/memres_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        ":memres_cpp",
        "//score/language/futurecpp",
        "@googletest//:gtest_main",
    ],
)

rust_test(
    name = "memres_rust_test",
    srcs = ["test/memres_rust_test.rs"],
    crate_root = "test/memres_rust_test.rs",
    edition = "2021",
    features = ["link_std_cpp_lib"],
    tags = ["unit"],
    deps = [
        ":memres",
        "//score/language/rust/memres/example:example_helpers",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":memres_test",
    ],
    visibility = ["//visibility:public"],
)
