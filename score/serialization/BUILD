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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "serialization",
    srcs = [
        "Serialization.cpp",
        "serializer.cpp",
    ],
    hdrs = [
        "Serialization.h",
        "serializer.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        ":strategy_interface",
        "//score/language/futurecpp",
        "//score/result",
        "//score/serialization/details",
        "//score/serialization/error",
        "//score/serialization/strategies:unsigned_integral_only",
        "//score/static_reflection_with_serialization/visitor",
    ],
)

cc_library(
    name = "strategy_interface",
    srcs = [
        "meta_information.cpp",
        "serialize_iterator.cpp",
    ],
    hdrs = [
        "meta_information.h",
        "serialize_iterator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "//score/language/futurecpp",
        "//score/result",
        "//score/serialization/details:type_traits",
        "//score/serialization/error",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "serialize_iterator_test.cpp",
        "serializer_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":serialization",
        "//score/serialization/test_doubles",
        "@googletest//:gtest_main",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":unit_test",
        "//score/serialization/error:unit_test",
        "//score/serialization/strategies:unit_test",
        "//score/serialization/strategies:unit_test_hex_string_to_bytes",
        "//score/serialization/test/unit:Serialization_test",
        "//score/serialization/types:unit_test",
    ],
    visibility = ["//visibility:public"],
)
