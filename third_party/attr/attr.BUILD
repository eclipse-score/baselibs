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

""" The BUILD file for the bazel_attr module. This module provides a C library for 
    extended attributes (xattr) on Linux. It includes a configuration header and
    the source files for the library. The library is built as a static library and
    is visible to other modules. The configuration header is selected based on the
    platform, and the library depends on the platforms and rules_cc modules.
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "config",
    hdrs = select({
        "@platforms//os:linux": ["bazel/configs/linux/config.h"],
    }),
    strip_include_prefix = select({
        "@platforms//os:linux": "bazel/configs/linux",
    }),
)

cc_library(
    name = "attr_h",
    hdrs = [
        "include/attributes.h",
        "include/libattr.h",
    ],
    include_prefix = "attr",
    includes = ["include"],
    strip_include_prefix = "include",
)

cc_library(
    name = "includes",
    hdrs = [
        "include/error_context.h",
        "include/misc.h",
        "include/nls.h",
        "include/walk_tree.h",
    ],
    includes = ["include"],
)

cc_library(
    name = "attr",
    srcs = [
        "libattr/libattr.c",
    ],
    hdrs = [
        "libattr/libattr.h",
    ],
    copts = [
        "-DSYSCONFDIR=\\\"$(location :xattr.conf)\\\"",
        "-Wno-error=cast-qual",
        "-Wno-error=missing-prototypes",
    ],
    data = [":xattr.conf"],
    includes = ["libattr"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        ":attr_h",
        ":config",
        ":includes",
    ],
)
