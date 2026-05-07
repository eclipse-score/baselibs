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

load("@rules_cc//cc:defs.bzl", "cc_library")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "strict_warnings",
    "additional_warnings",
]

cc_library(
    name = "factory",
    srcs = [
        "i_hash_calculator_factory.cpp",
    ],
    hdrs = [
        "i_hash_calculator_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/hash:__subpackages__"],
    deps = [
        "@score_baselibs//score/hash/code/core",
        "@score_baselibs//score/json",
        "@score_baselibs//score/mw/log:frontend",
    ],
)

cc_library(
    name = "factory_mock",
    srcs = [],
    hdrs = [
        "hash_calculator_factory_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/hash:__subpackages__"],
    deps = [
        ":factory",
    ],
)
