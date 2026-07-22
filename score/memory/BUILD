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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

package(default_visibility = [
    "@score_baselibs//score/memory:__subpackages__",
])

# --------------------------------------------------------------------------------------------------------------
# Migration shims for string_literal, string_comparison_adaptor and split_string_view, which moved to
# //score/string_manipulation.
#
# The headers below only forward-include the new headers and (except for string_literal, whose StringLiteral
# symbol already lives directly in the score namespace) pull the relocated symbols back into score::memory via
# using-declarations, so existing bazel targets, include paths and namespace usages of consumers outside of this
# package keep working unchanged. New code should depend on @score_baselibs//score/string_manipulation directly.
# --------------------------------------------------------------------------------------------------------------

cc_library(
    name = "string_literal",
    srcs = ["string_literal.cpp"],
    hdrs = ["string_literal.h"],
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/string_manipulation:string_literal instead."),
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/string_manipulation:string_literal",
    ],
)

cc_library(
    name = "string_comparison_adaptor",
    srcs = [
        "string_comparison_adaptor.cpp",
    ],
    hdrs = [
        "string_comparison_adaptor.h",
    ],
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/string_manipulation:string_comparison_adaptor instead."),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/string_manipulation:string_comparison_adaptor",
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
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/string_manipulation:split_string_view instead."),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/string_manipulation:split_string_view",
    ],
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
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "pmr_ring_buffer",
    hdrs = [
        "pmr_ring_buffer.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
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
        ":endianness_unit_test",
        ":pmr_ring_buffer_unit_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/memory/shared:unit_test_suite",
    ],
    visibility = [
        "//visibility:public",
    ],
)

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
    ],
    tidy_config_file = ".clang-tidy-extra",
)
