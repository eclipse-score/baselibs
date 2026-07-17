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

cc_library(
    name = "acl",
    srcs = select({
        "@platforms//cpu:aarch64": ["usr/lib/aarch64-linux-gnu/libacl.a"],
        "@platforms//cpu:x86_64": ["usr/lib/x86_64-linux-gnu/libacl.a"],
    }),
    hdrs = [
        "usr/include/acl/libacl.h",
        "usr/include/sys/acl.h",
    ],
    includes = ["usr/include/"],
    visibility = ["//visibility:public"],
)
