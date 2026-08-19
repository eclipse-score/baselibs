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

load("@rules_cc//cc:defs.bzl", "cc_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_test(
    name = "vajson_impl_unit_tests",
    srcs = glob(["**/*.cpp"]),
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    local_defines = ["VAJSON"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__subpackages__",
    ],
    deps = [
        "//score/json/internal/parser/vajson:vajson_parser",
        "@googletest//:gtest_main",
    ],
)
