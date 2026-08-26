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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

# Declarations of the serialization backend seam. Exactly one backend provides the definitions; which one is
# selected by the `//score/json:writer_library` flag below.
cc_library(
    name = "writer_backend",
    hdrs = ["writer_backend.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json:__subpackages__"],
    deps = [
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/result",
    ],
)

alias(
    name = "writer",
    actual = select({
        "@score_baselibs//score/json:writer_library_json_serialize": "@score_baselibs//score/json/internal/writer/json_serialize:json_serialize_backend",
        "@score_baselibs//score/json:writer_library_vajson": "@score_baselibs//score/json/internal/writer/vajson:vajson_backend",
    }),
    visibility = ["@score_baselibs//score/json:__pkg__"],
)
