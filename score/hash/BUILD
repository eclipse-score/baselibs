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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

cc_library(
    name = "hash",
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/hash/code/core/factory/impl:crc_variant",
    ],
)

cc_library(
    name = "safe_hash",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/hash/code/core/factory/impl:safe_crc_variant",
    ],
)

cc_library(
    name = "hash_mock",
    testonly = True,
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/hash/code/core:hash_cal_mock",
        "@score_baselibs//score/hash/code/core/factory:factory_mock",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    test_suites_from_sub_packages = [
        "@score_baselibs//score/hash/code/common:unit_tests",
        "@score_baselibs//score/hash/code/core:unit_tests",
        "@score_baselibs//score/hash/code/crc:unit_tests",
        "@score_baselibs//score/hash/code/openssl:unit_tests",
    ],
    visibility = ["//visibility:public"],  # platform_only
)
