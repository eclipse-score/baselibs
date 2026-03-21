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

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "offset_ptr",
    hdrs = ["offset_ptr.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "memory_resource",
    srcs = ["memory_resource.cpp"],
    hdrs = ["memory_resource.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "monotonic_buffer_resource",
    srcs = ["monotonic_buffer_resource.cpp"],
    hdrs = ["monotonic_buffer_resource.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":memory_resource",
    ],
)

cc_library(
    name = "container_error",
    srcs = ["container_error.cpp"],
    hdrs = ["container_error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "vector",
    hdrs = ["vector.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":container_error",
        ":memory_resource",
        ":offset_ptr",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "map",
    hdrs = ["map.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":container_error",
        ":memory_resource",
        ":offset_ptr",
        "@score_baselibs//score/result",
    ],
)

cc_gtest_unit_test(
    name = "memory_resource_test",
    srcs = ["memory_resource_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":memory_resource",
    ],
)

cc_gtest_unit_test(
    name = "monotonic_buffer_resource_test",
    srcs = ["monotonic_buffer_resource_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":memory_resource",
        ":monotonic_buffer_resource",
    ],
)

cc_gtest_unit_test(
    name = "vector_test",
    srcs = ["vector_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":container_error",
        ":memory_resource",
        ":vector",
    ],
)

cc_gtest_unit_test(
    name = "map_test",
    srcs = ["map_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":container_error",
        ":map",
        ":memory_resource",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":map_test",
        ":memory_resource_test",
        ":monotonic_buffer_resource_test",
        ":vector_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_binary(
    name = "offset_ptr_benchmark",
    testonly = True,
    srcs = ["offset_ptr_benchmark.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":vector",
        "@google_benchmark//:benchmark_main",
        "@score_baselibs//score/memory/shared:new_delete_delegate_resource",
        "@score_baselibs//score/memory/shared:offset_ptr",
        "@score_baselibs//score/memory/shared:vector",
        "@score_baselibs//score/memory/shared/fake:fake_memory_resources",
        "@score_baselibs//score/mw/log:console_only_backend",
    ],
)
