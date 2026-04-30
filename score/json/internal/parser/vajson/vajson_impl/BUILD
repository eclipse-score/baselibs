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

package(default_visibility = ["@score_baselibs//score/json:__subpackages__"])

cc_library(
    name = "vajson_impl",
    srcs = [
        "reader/internal/json_ops.cpp",
        "reader/internal/parsers/structure_parser_base.cpp",
        "reader/internal/parsers/virtual_parser.cpp",
        "reader/json_data.cpp",
        "reader/json_parser.cpp",
    ],
    hdrs = [
        "reader.h",
        "reader/internal/config/json_reader_cfg.h",
        "reader/internal/depth_counter.h",
        "reader/internal/json_ops.h",
        "reader/internal/level_validator.h",
        "reader/internal/parsers/array_parser.h",
        "reader/internal/parsers/bool_parser.h",
        "reader/internal/parsers/composition_parser.h",
        "reader/internal/parsers/composition_parser_impl.h",
        "reader/internal/parsers/key_parser.h",
        "reader/internal/parsers/number_parser.h",
        "reader/internal/parsers/object_parser.h",
        "reader/internal/parsers/string_parser.h",
        "reader/internal/parsers/structure_parser.h",
        "reader/internal/parsers/structure_parser_base.h",
        "reader/internal/parsers/structure_parser_impl.h",
        "reader/internal/parsers/virtual_parser.h",
        "reader/json_data.h",
        "reader/json_parser.h",
        "reader/parser.h",
        "reader/parser_state.h",
        "reader/single_array_parser.h",
        "reader/single_object_parser.h",
        "reader/v2.h",
        "reader/v2/parser.h",
        "reader/v2/single_array_parser.h",
        "reader/v2/single_object_parser.h",
        "reader_fwd.h",
        "util/json_error_domain.h",
        "util/number.h",
        "util/types.h",
    ],
    includes = [
        "include",
    ],
    deps = [
        "@score_baselibs//score/filesystem",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/language/safecpp/string_view:zstring_view",
        "@score_baselibs//score/result",
    ],
)
