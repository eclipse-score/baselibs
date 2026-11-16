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
load(":defs.bzl", "score_component_files")

# Demo entry points: the public libraries of the three components that carry
# metadata on this branch. In the real thing this list is the coverage scope
# (or simply //score/...), and the components fall out of the metadata.
DEMO_ENTRY_POINTS = [
    "//score/json",
    "//score/json:json_serializer",
    "//score/memory:data_type_size_info",
    "//score/memory:endianness",
    "//score/memory:pmr_ring_buffer",
    "//score/result",
    "//score/result:result_for_rust",
    "//score/result/rust:result_cxx",
    "//score/result/rust:result_rs",
    "//score/result/rust:rust",
]

# All components reached from the entry points, plus "(unassigned)" for files
# whose owner target carries no component (dependencies outside the demo).
score_component_files(
    name = "component_files",
    deps = DEMO_ENTRY_POINTS,
)

# One component only.
score_component_files(
    name = "json_files",
    component = "//score/json:component",
    deps = DEMO_ENTRY_POINTS,
)

score_component_files(
    name = "vajson_files",
    component = "//score/json:vajson_component",
    deps = DEMO_ENTRY_POINTS,
)
