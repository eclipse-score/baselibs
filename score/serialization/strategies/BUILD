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
    name = "unsigned_integral_only",
    srcs = ["unsigned_integral_only_strategy.cpp"],
    hdrs = ["unsigned_integral_only_strategy.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    # can be extended if users directly want to depend on it (encouraged)
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "//score/language/futurecpp",
        "//score/serialization:strategy_interface",
    ],
)

cc_library(
    name = "uds_strategy",
    srcs = ["uds_strategy.cpp"],
    hdrs = ["uds_strategy.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    # can be extended if users directly want to depend on it (encouraged)
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        ":unsigned_integral_only",
        "//score/memory:endianness",
        "//score/serialization:strategy_interface",
        "//score/serialization/types:length_prefixed_vector",
    ],
)

cc_library(
    name = "hex_string_to_bytes",
    srcs = [
        "hex_string_to_bytes.cpp",
    ],
    hdrs = [
        "hex_string_to_bytes.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = ["//score/language/futurecpp"],
)

cc_library(
    name = "uds_strategy_serialization_test_helper",
    srcs = ["uds_strategy_serialization_test_helper.cpp"],
    hdrs = ["uds_strategy_serialization_test_helper.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["//score:__subpackages__"],
    deps = [
        ":hex_string_to_bytes",
        ":uds_strategy",
        "//score/language/futurecpp",
        "//score/serialization",
        "//score/static_reflection_with_serialization/visitor",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "unit_test_hex_string_to_bytes",
    srcs = [
        "hex_string_to_bytes_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = ["//score/serialization:__pkg__"],
    deps = [
        ":hex_string_to_bytes",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "uds_strategy_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = ["//score/serialization:__pkg__"],
    deps = [
        ":uds_strategy",
        "//score/serialization",
        "//score/static_reflection_with_serialization/visitor",
        "@googletest//:gtest_main",
    ],
)
