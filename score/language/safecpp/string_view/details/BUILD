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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "zspan",
    srcs = ["zspan.cpp"],
    hdrs = ["zspan.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/language/safecpp/string_view:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/language/safecpp/string_view:null_termination_violation_policies",
    ],
)

cc_gtest_unit_test(
    name = "zspan_test",
    srcs = [
        "zspan_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "throws_upon_exception",
    ],
    visibility = [
        "@score_baselibs//score/language/safecpp/string_view:__pkg__",
    ],
    deps = [
        ":zspan",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/string_view:zstring_view",
    ],
)
