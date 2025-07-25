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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

package(default_visibility = [
    "@score-baselibs//score/memory:__subpackages__",
])

cc_library(
    name = "string_literal",
    hdrs = ["string_literal.h"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
)

cc_library(
    name = "any_string_view",
    srcs = ["any_string_view.cpp"],
    hdrs = ["any_string_view.h"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "endianness",
    srcs = ["endianness.cpp"],
    hdrs = ["endianness.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/lib/serialization:__subpackages__",
        "//platform/aas/pas/sysmon/library:__subpackages__",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "string_comparison_adaptor",
    srcs = [
        "string_comparison_adaptor.cpp",
    ],
    hdrs = [
        "string_comparison_adaptor.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":string_literal",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "string_comparison_adaptor_unit_test",
    srcs = [
        "string_comparison_adaptor_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":string_comparison_adaptor",
        "@googletest//:gtest_main",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "split_string_view",
    srcs = [
        "split_string_view.cpp",
    ],
    hdrs = [
        "split_string_view.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score-baselibs//score/mw/log:__subpackages__",
    ],
    deps = ["@score-baselibs//score/language/futurecpp"],
)

cc_library(
    name = "pmr_ring_buffer",
    hdrs = [
        "pmr_ring_buffer.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "split_string_view_unit_test",
    srcs = [
        "split_string_view_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":split_string_view",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "any_string_view_test",
    srcs = [
        "any_string_view_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":any_string_view",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "endianness_unit_test",
    srcs = [
        "endianness_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":endianness",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pmr_ring_buffer_unit_test",
    srcs = [
        "pmr_ring_buffer_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":pmr_ring_buffer",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":any_string_view_test",
        ":endianness_unit_test",
        ":pmr_ring_buffer_unit_test",
        ":split_string_view_unit_test",
        ":string_comparison_adaptor_unit_test",
        "@score-baselibs//score/memory/shared/flock:unit_test",
        "@score-baselibs//score/memory/shared:unit_test",
        "@score-baselibs//score/memory/shared:shared_memory_factory_test",
        "@score-baselibs//score/memory/shared:pointer_arithmetic_util_precondition_violation_test",
        "@score-baselibs//score/memory/shared:offset_ptr_precondition_violation_test",
        "@score-baselibs//score/memory/shared:offset_ptr_test",
        "@score-baselibs//score/memory/shared/sealedshm/sealedshm_wrapper:unit_test",
        "@score-baselibs//score/memory/shared:memory_region_bounds_test",
        "@score-baselibs//score/memory/shared/typedshm/typedshm_wrapper:typed_memory_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)
