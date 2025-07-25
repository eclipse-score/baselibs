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
load("@score-baselibs//bazel:unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "scope",
    srcs = ["scope.cpp"],
    hdrs = ["scope.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score-baselibs//score/language/safecpp/scoped_function/details:allocator_wrapper",
        "@score-baselibs//score/language/safecpp/scoped_function/details:scope_state",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "scope_test",
    srcs = ["scope_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":copyable_scoped_function",
        ":move_only_scoped_function",
        ":scope",
        "@score-baselibs//score/language/safecpp/scoped_function/details:instrumented_memory_resource",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "copyable_scoped_function",
    srcs = ["copyable_scoped_function.cpp"],
    hdrs = ["copyable_scoped_function.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":scope",
        "@score-baselibs//score/language/safecpp/scoped_function/details:allocator_aware_type_erasure_pointer",
        "@score-baselibs//score/language/safecpp/scoped_function/details:function_wrapper",
        "@score-baselibs//score/language/safecpp/scoped_function/details:invoker",
        "@score-baselibs//score/language/safecpp/scoped_function/details:is_callable_from",
        "@score-baselibs//score/language/safecpp/scoped_function/details:modify_signature",
        "@score-baselibs//score/language/safecpp/scoped_function/details:scope_state",
        "@score-baselibs//score/language/safecpp/scoped_function/details:scoped_function_invoker",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "move_only_scoped_function",
    srcs = ["move_only_scoped_function.cpp"],
    hdrs = ["move_only_scoped_function.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":copyable_scoped_function",
        ":scope",
        "@score-baselibs//score/language/safecpp/scoped_function/details:allocator_aware_type_erasure_pointer",
        "@score-baselibs//score/language/safecpp/scoped_function/details:function_wrapper",
        "@score-baselibs//score/language/safecpp/scoped_function/details:invoker",
        "@score-baselibs//score/language/safecpp/scoped_function/details:is_callable_from",
        "@score-baselibs//score/language/safecpp/scoped_function/details:modify_signature",
        "@score-baselibs//score/language/safecpp/scoped_function/details:scope_state",
        "@score-baselibs//score/language/safecpp/scoped_function/details:scoped_function_invoker",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "scoped_function_test",
    srcs = [
        "scoped_function_basic_test.cpp",
        "scoped_function_copyability_test.cpp",
        "scoped_function_invocation_test.cpp",
        "scoped_function_movability_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":copyable_scoped_function",
        ":move_only_scoped_function",
        "@score-baselibs//score/language/safecpp/scoped_function/details:instrumented_memory_resource",
        "@score-baselibs//score/language/safecpp/scoped_function/details:parametrization_helper",
        "@score-baselibs//score/language/safecpp/scoped_function/details:testing_allocator",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        "@score-baselibs//score/language/safecpp/scoped_function:scope_test",
        "@score-baselibs//score/language/safecpp/scoped_function:scoped_function_test",
    ],
    test_suites_from_sub_packages = ["@score-baselibs//score/language/safecpp/scoped_function/details:unit_tests"],
    visibility = ["@score-baselibs//score/language/safecpp:__pkg__"],
)
