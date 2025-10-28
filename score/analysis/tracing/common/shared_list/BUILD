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
    name = "shared_list",
    srcs = [
        "shared_list.cpp",
    ],
    hdrs = [
        "shared_list.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FUSA"],
    visibility = [
        "@score_baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator:flexible_circular_allocator_interface",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types/error_code",
    ],
)
