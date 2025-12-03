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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_test(
    name = "shared_list_test",
    srcs = [
        "shared_list_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator",
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator/test/mocks:flexible_circular_allocator_mock",
        "@score_baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score_baselibs//score/analysis/tracing/common/shared_list",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":shared_list_test",
    ],
    visibility = ["@score_baselibs//score/analysis/tracing:__subpackages__"],
)
