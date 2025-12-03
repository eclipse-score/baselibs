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

# load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "custom_allocator_mock",
    testonly = True,
    srcs = ["custom_allocator_mock.cpp"],
    hdrs = ["custom_allocator_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/memory/shared:offset_ptr",
    ],
)

cc_library(
    name = "allocator_test_type_helpers",
    testonly = True,
    srcs = ["allocator_test_type_helpers.cpp"],
    hdrs = ["allocator_test_type_helpers.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
    deps = [
        ":container_test_types",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/memory/shared:managed_memory_resource",
        "@score_baselibs//score/memory/shared:polymorphic_offset_ptr_allocator",
    ],
)

cc_library(
    name = "container_test_types",
    testonly = True,
    srcs = ["container_test_types.cpp"],
    hdrs = ["container_test_types.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
)
