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

cc_test(
    name = "shared_list_test",
    srcs = [
        "shared_list_test.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator",
        "@score-baselibs//score/analysis/tracing/common/flexible_circular_allocator/test/mocks:flexible_circular_allocator_mock",
        "@score-baselibs//score/analysis/tracing/common/interface_types:shared_memory_location_helpers",
        "@score-baselibs//score/analysis/tracing/common/shared_list",
        "@googletest//:gtest_main",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":shared_list_test",
    ],
    visibility = ["@score-baselibs//score/analysis/tracing:__subpackages__"],
)
