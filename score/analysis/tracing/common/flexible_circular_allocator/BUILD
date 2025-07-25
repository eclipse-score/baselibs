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
    name = "flexible_circular_allocator",
    srcs = [
        "custom_polymorphic_offset_ptr_allocator.cpp",
        "flexible_circular_allocator.cpp",
    ],
    hdrs = [
        "custom_polymorphic_offset_ptr_allocator.h",
        "flexible_circular_allocator.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "flexible_circular_allocator_interface",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "lockless_flexible_circular_allocator",
    srcs = [
        "custom_polymorphic_offset_ptr_allocator.cpp",
        "lockless_flexible_circular_allocator.cpp",
    ],
    hdrs = [
        "custom_polymorphic_offset_ptr_allocator.h",
        "lockless_flexible_circular_allocator.h",
        "lockless_flexible_circular_allocator_types.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "flexible_circular_allocator_interface",
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/analysis/tracing/library/generic_trace_api/error_code",
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "flexible_circular_allocator_interface",
    srcs = [
        "flexible_circular_allocator_interface.cpp",
    ],
    hdrs = [
        "flexible_circular_allocator_interface.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/analysis/tracing/common/interface_types:generic_trace_api_types",
        "@score-baselibs//score/memory/shared:types",
        "@score-baselibs//score/mw/log",
    ],
)

cc_library(
    name = "flexible_circular_allocator_factory",
    hdrs = [
        "flexible_circular_allocator_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "error_code",
        "flexible_circular_allocator",
        "flexible_circular_allocator_factory_interface",
    ],
)

cc_library(
    name = "lockless_flexible_circular_allocator_factory",
    hdrs = [
        "lockless_flexible_circular_allocator_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "error_code",
        "flexible_circular_allocator_factory_interface",
        "lockless_flexible_circular_allocator",
    ],
)

cc_library(
    name = "flexible_circular_allocator_factory_interface",
    hdrs = [
        "flexible_circular_allocator_factory_interface.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "error_code",
        "flexible_circular_allocator_interface",
    ],
)

cc_library(
    name = "error_code",
    srcs = [
        "error_code.cpp",
    ],
    hdrs = [
        "error_code.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score-baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score-baselibs//score/result",
        "@score-baselibs//score/language/futurecpp",
    ],
)
