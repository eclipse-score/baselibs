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
cc_library(
    name = "generic_trace_api_types",
    srcs = [
        "types.cpp",
    ],
    hdrs = [
        "types.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/language/safecpp/scoped_function:move_only_scoped_function",
        "@score_baselibs//score/memory/shared",
        "@score_baselibs//score/result",
    ],
)

cc_library(
    name = "shared_memory_location",
    srcs = ["shared_memory_location.cpp"],
    hdrs = ["shared_memory_location.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing:__subpackages__",
    ],
    deps = [
        ":generic_trace_api_types",
    ],
)

cc_library(
    name = "shared_memory_location_helpers",
    srcs = ["shared_memory_location_helpers.cpp"],
    hdrs = ["shared_memory_location_helpers.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing:__subpackages__",
    ],
    deps = [
        ":generic_trace_api_types",
        ":shared_memory_chunk",
        ":shared_memory_location",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types/error_code",
    ],
)

cc_library(
    name = "shared_memory_chunk",
    srcs = ["shared_memory_chunk.cpp"],
    hdrs = ["shared_memory_chunk.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing:__subpackages__",
    ],
    deps = [
        ":generic_trace_api_types",
        ":shared_memory_location",
    ],
)
