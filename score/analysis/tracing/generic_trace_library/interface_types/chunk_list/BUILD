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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "chunk_list",
    srcs = [
        "local_data_chunk_list.cpp",
        "shm_data_chunk_list.cpp",
    ],
    hdrs = [
        "local_data_chunk_list.h",
        "shm_data_chunk_list.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator:lockless_flexible_circular_allocator_factory",
        "@score_baselibs//score/analysis/tracing/common/shared_list",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/interface_types/error_code",
        "@score_baselibs//score/language/futurecpp",
    ],
)
