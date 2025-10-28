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

cc_gtest_unit_test(
    name = "intrusive_list_test",
    srcs = ["intrusive_list_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [":intrusivelist"],
)

cc_gtest_unit_test(
    name = "dynamic_array_test",
    srcs = ["dynamic_array_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":dynamic_array",
        "@score_baselibs//score/memory/shared:fake_memory_resources",
        "@score_baselibs//score/memory/shared:types",
        "@score_baselibs//score/mw/log",
    ],
)

cc_library(
    name = "dynamic_array",
    srcs = ["dynamic_array.cpp"],
    hdrs = ["dynamic_array.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
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
        "//visibility:public", # platform_only
    ],
    deps = ["@score_baselibs//score/language/futurecpp"],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":dynamic_array_test",
        ":intrusive_list_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)
