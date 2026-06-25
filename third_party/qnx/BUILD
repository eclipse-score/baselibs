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
    name = "libfsnotify",
    linkopts = [
        "-lfsnotify",
    ],
    tags = ["third_party_FUSA"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//:__subpackages__"],
)

cc_library(
    name = "libsecpolev",
    linkopts = [
        "-lsecpolev",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//:__subpackages__"],
)

cc_library(
    name = "openssl",
    linkopts = [
        "-lssl",
        "-lcrypto",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//:__subpackages__"],
)
