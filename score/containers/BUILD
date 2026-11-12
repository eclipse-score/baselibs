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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_docs_as_code//:docs.bzl", "docs_bundle")

cc_gtest_unit_test(
    name = "intrusive_list_test",
    srcs = ["intrusive_list_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":intrusivelist",
        "@score_baselibs//score/quality/compiler_warnings",
    ],
)

cc_unit_test(
    name = "dynamic_array_test",
    srcs = ["dynamic_array_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        # These tests catch exceptions instead of using gtest EXPECT_DEATH so we disable aborts_upon_exception.
        "-aborts_upon_exception",
    ],
    deps = [
        ":dynamic_array",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/containers/test:allocator_test_type_helpers",
        "@score_baselibs//score/containers/test:container_test_types",
        "@score_baselibs//score/containers/test:fake_memory_resource",
        "@score_baselibs//score/containers/test:fancy_pointer_allocator",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test(
    name = "non_relocatable_vector_test",
    srcs = ["non_relocatable_vector_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        # These tests catch exceptions instead of using gtest EXPECT_DEATH so we disable aborts_upon_exception.
        "-aborts_upon_exception",
    ],
    deps = [
        ":non_relocatable_vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/containers/test:allocator_test_type_helpers",
        "@score_baselibs//score/containers/test:container_test_types",
        "@score_baselibs//score/containers/test:fake_memory_resource",
        "@score_baselibs//score/containers/test:fancy_pointer_allocator",
        "@score_baselibs//score/containers/test:mockable_allocator",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test(
    name = "non_relocatable_vector_special_member_funcs_test",
    srcs = ["non_relocatable_vector_special_member_funcs_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        # These tests catch exceptions instead of using gtest EXPECT_DEATH so we disable aborts_upon_exception.
        "-aborts_upon_exception",
    ],
    deps = [
        ":non_relocatable_vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/containers/test:allocator_test_type_helpers",
        "@score_baselibs//score/containers/test:container_test_types",
        "@score_baselibs//score/containers/test:fake_memory_resource",
        "@score_baselibs//score/containers/test:fancy_pointer_allocator",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test(
    name = "non_relocatable_vector_emplace_back_test",
    srcs = ["non_relocatable_vector_emplace_back_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        # These tests catch exceptions instead of using gtest EXPECT_DEATH so we disable aborts_upon_exception.
        "-aborts_upon_exception",
    ],
    deps = [
        ":non_relocatable_vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/containers/test:allocator_test_type_helpers",
        "@score_baselibs//score/containers/test:container_test_types",
        "@score_baselibs//score/containers/test:fake_memory_resource",
        "@score_baselibs//score/containers/test:fancy_pointer_allocator",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test(
    name = "non_relocatable_vector_capacity_test",
    srcs = ["non_relocatable_vector_capacity_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        # These tests catch exceptions instead of using gtest EXPECT_DEATH so we disable aborts_upon_exception.
        "-aborts_upon_exception",
    ],
    deps = [
        ":non_relocatable_vector",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@score_baselibs//score/containers/test:allocator_test_type_helpers",
        "@score_baselibs//score/containers/test:container_test_types",
        "@score_baselibs//score/containers/test:fake_memory_resource",
        "@score_baselibs//score/containers/test:fancy_pointer_allocator",
        "@score_baselibs//score/language/futurecpp:futurecpp_test_support",
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_library(
    name = "dynamic_array",
    srcs = ["dynamic_array.cpp"],
    hdrs = ["dynamic_array.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "non_relocatable_vector",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "non_relocatable_vector",
    srcs = ["non_relocatable_vector.cpp"],
    hdrs = ["non_relocatable_vector.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "intrusivelist",
    srcs = ["intrusive_list.cpp"],
    hdrs = ["intrusive_list.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "string_registry",
    srcs = [
        "string_registry.cpp",
    ],
    hdrs = [
        "string_registry.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [":registry_string_view"],
)

cc_unit_test(
    name = "string_registry_unit_test",
    srcs = [
        "string_registry_tests.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    deps = [
        ":string_registry",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "registry_string_view",
    hdrs = [
        "registry_string_view.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
)

cc_unit_test(
    name = "registry_string_view_unit_tests",
    srcs = [
        "registry_string_view_tests.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    deps = [
        ":registry_string_view",
        ":string_registry",
        "@googletest//:gtest_main",
    ],
)

docs_bundle(
    name = "docs",
    code_targets = [
        ":dynamic_array",
        ":intrusivelist",
        ":non_relocatable_vector",
    ],
    primary_need_id = "comp__baselibs_containers",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)
