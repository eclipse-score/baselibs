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
    name = "compiler_warnings",
    srcs = ["warnings.cpp"],
    hdrs = ["warnings.h"],
    tags = ["FFI"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "macro_deprecation",
    hdrs = ["deprecation.h"],
    tags = ["FFI"],
    visibility = ["@score_baselibs//score:__subpackages__"],
)
