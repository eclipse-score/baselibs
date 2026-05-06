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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load(":custom.bzl", "load_custom_targets", "load_custom_test_suites")

cc_library(
    name = "crc_ieee",
    srcs = ["crc32_ieee.cpp"],
    hdrs = [
        "crc32_ieee.h",
        "i_crc32.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/hash:__pkg__",
        "@score_baselibs//score/hash/code/core/factory/impl:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/hash/code/core",
    ],
)

cc_library(
    name = "mocks",
    hdrs = [
        "crc32_mock.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["@score_baselibs//score/hash/code/core/factory/impl:__pkg__"],
    deps = [
        ":crc_ieee",
    ],
)

cc_test(
    name = "unit_test_ieee",
    srcs = ["crc32_ieee_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/hash:__pkg__"],
    deps = [
        ":crc_ieee",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":unit_test_ieee",
    ] + load_custom_test_suites(),
    visibility = ["//visibility:public"],  # platform_only
)

load_custom_targets()
