# ******************************************************************************

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

# ******************************************************************************

load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
name = "generic_trace_api_header",
hdrs = ["generic_trace_api.h"],
features = COMPILER_WARNING_FEATURES,
tags = ["FFI"],



visibility = ["//visibility:public"],
deps = [
    ":meta_info",
    "@score-baselibs//score/analysis/tracing/generic_trace_library/interface_types/chunk_list",
    "@score-baselibs//score/analysis/tracing/generic_trace_library/interface_types/error_code",
    "@score-baselibs//score/language/futurecpp",
    # generic_trace_api.h includes "i_trace_library.h" → provide it transitively
    ":trace_library_interface",
],



)

cc_library(
name = "trace_library_interface",
hdrs = ["i_trace_library.h"],
features = COMPILER_WARNING_FEATURES,
tags = ["FFI"],




visibility = ["//visibility:public"],
deps = [
    ":meta_info",
    "@score-baselibs//score/analysis/tracing/generic_trace_library/interface_types/chunk_list",
    "@score-baselibs//score/analysis/tracing/generic_trace_library/interface_types/error_code",
    "@score-baselibs//score/concurrency",
    "@score-baselibs//score/language/futurecpp",
],



)

cc_library(
name = "meta_info",
srcs = [
"ara_com_meta_info.cpp",
"ara_com_properties.cpp",
],
hdrs = [
"ara_com_meta_info.h",
"ara_com_properties.h",
"dlt_meta_info.h",
"dlt_properties.h",
"meta_info.h",
"meta_info_variants.h",
],
features = COMPILER_WARNING_FEATURES,
tags = ["FFI"],
visibility = ["//visibility:public"],
deps = [
":type_definitions",
"@score-baselibs//score/memory:string_literal",
"@score-baselibs//score/language/futurecpp",
],
)

cc_library(
name = "type_definitions",
hdrs = [
"service_instance_element.h",
"trace_point_type.h",
],
features = COMPILER_WARNING_FEATURES,
tags = ["FFI"],
visibility = ["//visibility:public"],
)