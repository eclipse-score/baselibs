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
    name = "vajson",
    srcs = [
        "reader/internal/json_ops.cpp",
        "reader/internal/parsers/structure_parser_base.cpp",
        "reader/internal/parsers/virtual_parser.cpp",
        "reader/json_data.cpp",
    ],
    hdrs = [
        "reader/internal/config/json_reader_cfg.h",
        "reader/internal/depth_counter.h",
        "reader/internal/json_ops.h",
        "reader/internal/parsers/structure_parser.h",
        "reader/internal/parsers/structure_parser_base.h",
        "reader/internal/parsers/structure_parser_impl.h",
        "reader/internal/parsers/virtual_parser.h",
        "reader/json_data.h",
        "reader/parser_state.h",
        "reader_fwd.h",
        "util/number.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/filesystem/filestream",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/json:__pkg__",
        "@score_baselibs//score/json/internal/parser:__pkg__",
        "@score_baselibs//score/json/internal/parser/vajson/unit_test:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/json/internal/common",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/result",
    ],
)
