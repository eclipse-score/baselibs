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

cc_library(
    name = "any_serializer",
    hdrs = [
        "any_serializer.h",
        "escaped_json_string.h",
        "formatting_writer.h",
        "generic_value_serializer.h",
        "j_array_type.h",
        "j_bool_type.h",
        "j_key_type.h",
        "j_null_type.h",
        "j_number_type.h",
        "j_object_type.h",
        "j_string_type.h",
        "key_serializer.h",
        "serializer_types.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/json/internal/writer:__pkg__"],
    deps = [
        "@score_baselibs//score/json:formatting",
        "@score_baselibs//score/json/internal/common",
        "@score_baselibs//score/json/internal/model",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/string_manipulation:string_comparison_adaptor",
    ],
)
