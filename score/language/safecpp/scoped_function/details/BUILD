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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "allocator_wrapper",
    srcs = ["allocator_wrapper.cpp"],
    hdrs = ["allocator_wrapper.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "allocator_wrapper_test",
    srcs = ["allocator_wrapper_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":allocator_wrapper",
        ":testing_allocator",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/quality/compiler_warnings",
    ],
)

cc_library(
    name = "allocator_aware_erased_type",
    srcs = ["allocator_aware_erased_type.cpp"],
    hdrs = ["allocator_aware_erased_type.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = [
        ":type_erasure_pointer",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "allocator_aware_type_erasure_pointer",
    srcs = ["allocator_aware_type_erasure_pointer.cpp"],
    hdrs = ["allocator_aware_type_erasure_pointer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = [
        ":allocator_aware_erased_type",
        ":allocator_wrapper",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "allocator_aware_type_erasure_pointer_test",
    srcs = ["allocator_aware_type_erasure_pointer_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":allocator_aware_type_erasure_pointer",
        ":instrumented_memory_resource",
        ":testing_allocator",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "call_and_return_modified",
    srcs = ["call_and_return_modified.cpp"],
    hdrs = ["call_and_return_modified.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = [
        ":modify_return_type",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "call_and_return_modified_test",
    srcs = ["call_and_return_modified_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":call_and_return_modified",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "function_wrapper",
    srcs = ["function_wrapper.cpp"],
    hdrs = ["function_wrapper.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
    deps = [
        ":allocator_aware_erased_type",
        ":invoker",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "function_wrapper_test",
    srcs = ["function_wrapper_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":function_wrapper",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/scoped_function/details:instrumented_memory_resource",
    ],
)

cc_library(
    name = "instrumented_memory_resource",
    testonly = True,
    srcs = ["instrumented_memory_resource.cpp"],
    hdrs = ["instrumented_memory_resource.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "invoker",
    srcs = ["invoker.cpp"],
    hdrs = ["invoker.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_library(
    name = "is_callable_from",
    srcs = ["is_callable_from.cpp"],
    hdrs = ["is_callable_from.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "is_callable_from_test",
    srcs = [
        "is_callable_from_test.cpp",
        "is_callable_from_with_compatible_modifiers_test.cpp",
        "is_callable_from_with_wrong_modifiers_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [":is_callable_from"],
)

cc_library(
    name = "modify_return_type",
    srcs = ["modify_return_type.cpp"],
    hdrs = ["modify_return_type.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "modify_return_type_test",
    srcs = ["modify_return_type_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":modify_return_type",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "modify_signature",
    srcs = ["modify_signature.cpp"],
    hdrs = ["modify_signature.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = [
        ":modify_return_type",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "modify_signature_test",
    srcs = ["modify_signature_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":modify_signature",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "parametrization_helper",
    testonly = True,
    srcs = ["parametrization_helper.cpp"],
    hdrs = ["parametrization_helper.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
)

cc_library(
    name = "scope_state",
    srcs = ["scope_state.cpp"],
    hdrs = ["scope_state.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "scope_state_test",
    srcs = ["scope_state_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":scope_state",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/scoped_function/details:function_wrapper",
    ],
)

cc_library(
    name = "scoped_function_invoker",
    srcs = ["scoped_function_invoker.cpp"],
    hdrs = ["scoped_function_invoker.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
    deps = [
        ":call_and_return_modified",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "scoped_function_invoker_test",
    srcs = [
        "scoped_function_invoker_test.cpp",
        "scoped_function_invoker_test_scoped_function.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":allocator_aware_type_erasure_pointer",
        ":function_wrapper",
        ":invoker",
        ":modify_signature",
        ":scope_state",
        ":scoped_function_invoker",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "testing_allocator",
    testonly = True,
    srcs = ["testing_allocator.cpp"],
    hdrs = ["testing_allocator.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__subpackages__"],
)

cc_library(
    name = "type_erasure_pointer",
    srcs = ["type_erasure_pointer.cpp"],
    hdrs = ["type_erasure_pointer.h"],
    tags = ["FFI"],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_gtest_unit_test(
    name = "type_erasure_pointer_test",
    srcs = ["type_erasure_pointer_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":instrumented_memory_resource",
        ":type_erasure_pointer",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        "@score_baselibs//score/language/safecpp/scoped_function/details:allocator_aware_type_erasure_pointer_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:allocator_wrapper_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:function_wrapper_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:call_and_return_modified_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:is_callable_from_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:modify_return_type_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:modify_signature_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:scope_state_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:type_erasure_pointer_test",
        "@score_baselibs//score/language/safecpp/scoped_function/details:scoped_function_invoker_test",
    ],
    visibility = ["@score_baselibs//score/language/safecpp/scoped_function:__pkg__"],
)
