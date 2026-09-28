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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "json_serialize",
    srcs = [
        "json_serialize.cpp",
    ],
    hdrs = [
        "json_serialize.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "json_serialize_backend",
    srcs = ["json_serialize_backend.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json/internal/writer:__pkg__"],
    deps = [
        ":json_serialize",
        "@score_baselibs//score/json/internal/writer:writer_backend",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/safe_math",
    ],
)

cc_test(
    name = "json_serialize_unit_test",
    srcs = [
        "json_serialize_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
    ],
    deps = [
        ":json_serialize",
        "@googletest//:gtest_main",
    ],
)
