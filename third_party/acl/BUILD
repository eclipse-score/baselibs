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
    name = "acl_system",
    linkopts = ["-lacl"],
    visibility = ["//:__subpackages__"],
)

alias(
    name = "acl",
    actual = select({
        "//third_party:use_system_libs": ":acl_system",
        "@platforms//cpu:aarch64": "@acl-deb-aarch64//:acl",
        "//conditions:default": "@acl-deb//:acl",
    }),
    visibility = ["//:__subpackages__"],
)
