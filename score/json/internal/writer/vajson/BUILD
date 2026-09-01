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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "vajson_serialize",
    srcs = ["vajson_serialize.cpp"],
    hdrs = ["vajson_serialize.h"] + glob([
        "writer/**/*.h",
    ]),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/json/internal/parser/vajson/vajson_impl",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "vajson_backend",
    srcs = ["vajson_backend.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json/internal/writer:__pkg__"],
    deps = [
        ":vajson_serialize",
        "@score_baselibs//score/json/internal/writer:writer_backend",
    ],
)

cc_test(
    name = "vajson_serialize_test",
    srcs = ["vajson_serialize_test.cpp"],
    features = COMPILER_WARNING_FEATURES + ["aborts_upon_exception"],
    tags = ["unit"],
    deps = [
        ":vajson_serialize",
        "@googletest//:gtest_main",
    ],
)
