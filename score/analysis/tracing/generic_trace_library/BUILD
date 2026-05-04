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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_baselibs//score/quality/clang_tidy:extra_checks.bzl", "clang_tidy_extra_checks")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    test_suites_from_sub_packages = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/details/containers/atomic_container/unit_test:unit_tests",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/details/containers/atomic_ring_buffer/unit_test:unit_tests",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/test/unit_test:unit_tests",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "generic_trace_library",
    srcs = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/details:generic_trace_api",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara:__subpackages__",
        "//platform/aas/mw/com:__subpackages__",
        "@score_baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/details:generic_trace_api_impl",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api_header",
    ],
)

clang_tidy_extra_checks(
    name = "clang_tidy_extra_checks",
    extra_features = [
        "spp_code_style_check_header_guards",
        "spp_code_style_check_method_names",
        "spp_code_style_check_readability",
        "spp_code_style_check_type_names",
        "spp_code_style_check_variable_names",
    ],
    skip_qnx_only_targets = True,
)
