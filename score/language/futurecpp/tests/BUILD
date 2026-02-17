# *******************************************************************************
# Copyright (c) 2015 Contributors to the Eclipse Foundation
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

load("//score/language/futurecpp:copts.bzl", "SCORE_LANGUAGE_FUTURECPP_ASSERT_DEFINES", "SCORE_LANGUAGE_FUTURECPP_TEST_COPTS")

googletest_tests = [
    "algorithm_test",
    "algorithm_test_find_if_unique",
    "algorithm_test_map_to",
    "algorithm_test_transform_if",
    "apply_test",
    "assert_support_test",
    "assert_test",
    "bit_test",
    "blank_test",
    "charconv_test",
    "chrono_test",
    "circular_buffer_unit_test",
    "connect_test",
    "construct_at_test",
    "cpu_context_test",
    "data_test",
    "expected_test",
    "expected_type_test",
    "float_test",
    "for_each_tuple_element_test",
    "functional_test",
    "hash_test",
    "inplace_vector_test",
    "intrusive_forward_list_test",
    "invoke_traits_test",
    "iterator_test",
    "jthread_test",
    "latch_test",
    "math_test",
    "member_iterator_test",
    "memory_resource_test",
    "memory_test",
    "move_only_function_test",
    "multi_span_test",
    "narrow_test",
    "numeric_saturate_cast_test",
    "optional_test",
    "overload_test",
    "range_pair_test",
    "schedule_test",
    "set_stopped_test",
    "set_value_test",
    "simd_mask_test",
    "simd_math_test",
    "simd_vec_test",
    "size_test",
    "span_test",
    "spin_mutex_test",
    "start_test",
    "static_vector_test",
    "string_test",
    "string_view_test",
    "then_test",
    "thread_pool_queue_test",
    "thread_pool_test",
    "tuple_test",
    "type_map_test",
    "type_traits_comparable_test",
    "type_traits_test",
    "uninitialized_construct_using_allocator_test",
    "uninitialized_move_test",
    "uninitialized_value_construct_test",
    "uses_allocator_construction_args_test",
    "utility_test",
    "utility_in_range_test",
    "utility_int_cmp_test",
    "variant_test",
    "zip_iterator_test",
]

[cc_test(
    name = f,
    size = "small",
    srcs = ["%s.cpp" % f],
    copts = SCORE_LANGUAGE_FUTURECPP_TEST_COPTS,
    defines = SCORE_LANGUAGE_FUTURECPP_ASSERT_DEFINES,
    tags = ["unit"],
    visibility = ["//visibility:public"],
    deps = [
        "//score/language/futurecpp:futurecpp_internal",
        "@googletest//:gtest_main",
    ],
) for f in googletest_tests]

cc_test(
    name = "static_const_test",
    size = "small",
    srcs = [
        "static_const_test.cpp",
        "static_const_test_helper.cpp",
        "static_const_test_helper.hpp",
    ],
    copts = SCORE_LANGUAGE_FUTURECPP_TEST_COPTS,
    defines = SCORE_LANGUAGE_FUTURECPP_ASSERT_DEFINES,
    tags = ["unit"],
    visibility = ["//visibility:public"],
    deps = [
        "//score/language/futurecpp:futurecpp_internal",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "stop_token_test",
    size = "medium",
    srcs = [
        "stop_token_test.cpp",
    ],
    copts = SCORE_LANGUAGE_FUTURECPP_TEST_COPTS,
    defines = SCORE_LANGUAGE_FUTURECPP_ASSERT_DEFINES,
    tags = ["unit"],
    visibility = ["//visibility:public"],
    deps = [
        "//score/language/futurecpp:futurecpp_internal",
        "@googletest//:gtest_main",
    ],
)
