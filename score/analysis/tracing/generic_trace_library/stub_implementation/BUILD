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
    name = "stub_implementation",
    srcs = [
        "generic_trace_api_stub.cpp",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:generic_trace_api_header",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types:trace_library_interface",
        "@score_baselibs//score/language/futurecpp",
    ],
)
