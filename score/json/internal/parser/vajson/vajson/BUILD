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
    name = "vajson",
    srcs = [
        "src/amsr/json/reader/internal/json_ops.cpp",
        "src/amsr/json/reader/internal/parsers/structure_parser_base.cpp",
        "src/amsr/json/reader/internal/parsers/virtual_parser.cpp",
        "src/amsr/json/reader/json_data.cpp",
        "src/amsr/json/reader/json_parser.cpp",
    ],
    hdrs = [
        "include/amsr/json/reader.h",
        "include/amsr/json/reader/internal/config/json_reader_cfg.h",
        "include/amsr/json/reader/internal/depth_counter.h",
        "include/amsr/json/reader/internal/json_ops.h",
        "include/amsr/json/reader/internal/level_validator.h",
        "include/amsr/json/reader/internal/parsers/array_parser.h",
        "include/amsr/json/reader/internal/parsers/binary_parser.h",
        "include/amsr/json/reader/internal/parsers/bool_parser.h",
        "include/amsr/json/reader/internal/parsers/composition_parser.h",
        "include/amsr/json/reader/internal/parsers/composition_parser_impl.h",
        "include/amsr/json/reader/internal/parsers/key_parser.h",
        "include/amsr/json/reader/internal/parsers/number_parser.h",
        "include/amsr/json/reader/internal/parsers/object_parser.h",
        "include/amsr/json/reader/internal/parsers/string_parser.h",
        "include/amsr/json/reader/internal/parsers/structure_parser.h",
        "include/amsr/json/reader/internal/parsers/structure_parser_base.h",
        "include/amsr/json/reader/internal/parsers/structure_parser_impl.h",
        "include/amsr/json/reader/internal/parsers/virtual_parser.h",
        "include/amsr/json/reader/json_data.h",
        "include/amsr/json/reader/json_parser.h",
        "include/amsr/json/reader/parser.h",
        "include/amsr/json/reader/parser_state.h",
        "include/amsr/json/reader/single_array_parser.h",
        "include/amsr/json/reader/single_object_parser.h",
        "include/amsr/json/reader/v2.h",
        "include/amsr/json/reader/v2/parser.h",
        "include/amsr/json/reader/v2/single_array_parser.h",
        "include/amsr/json/reader/v2/single_object_parser.h",
        "include/amsr/json/reader_fwd.h",
        "include/amsr/json/util/json_error_domain.h",
        "include/amsr/json/util/number.h",
        "include/amsr/json/util/types.h",
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
