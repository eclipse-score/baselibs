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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "mockable_allocator",
    testonly = True,
    srcs = [
        "mockable_allocator.cpp",
        "mockable_pointer.cpp",
        "mockable_pointer_mock_guard.cpp",
        "pointer_spy_mock.cpp",
    ],
    hdrs = [
        "mockable_allocator.h",
        "mockable_pointer.h",
        "mockable_pointer_mock_guard.h",
        "pointer_spy_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
    deps = [
        "@googletest//:gtest_main",
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

cc_library(
    name = "fake_memory_resource",
    testonly = True,
    srcs = ["fake_memory_resource.cpp"],
    hdrs = ["fake_memory_resource.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "fake_memory_resource_test",
    srcs = ["fake_memory_resource_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":fake_memory_resource",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "fancy_pointer_allocator",
    testonly = True,
    srcs = ["fancy_pointer_allocator.cpp"],
    hdrs = ["fancy_pointer_allocator.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "@score_baselibs//score/containers:__subpackages__",
    ],
    deps = [
        ":mockable_allocator",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "fancy_pointer_allocator_test",
    srcs = ["fancy_pointer_allocator_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":fake_memory_resource",
        ":fancy_pointer_allocator",
        "@googletest//:gtest",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":fake_memory_resource_test",
        ":fancy_pointer_allocator_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)
