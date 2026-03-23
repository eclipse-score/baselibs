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

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "shm_parent",
    srcs = [
        "bounded_containers.h",
        "shm_parent.cpp",
    ],
    linkopts = ["-lrt"],
    deps = [
        "@score_baselibs//score/nothrow:map",
        "@score_baselibs//score/nothrow:monotonic_buffer_resource",
        "@score_baselibs//score/nothrow:vector",
    ],
)

cc_binary(
    name = "shm_child",
    srcs = [
        "bounded_containers.h",
        "shm_child.cpp",
    ],
    linkopts = ["-lrt"],
    deps = [
        "@score_baselibs//score/nothrow:map",
        "@score_baselibs//score/nothrow:monotonic_buffer_resource",
        "@score_baselibs//score/nothrow:vector",
    ],
)
