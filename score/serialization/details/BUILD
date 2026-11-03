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

cc_library(
    name = "type_traits",
    srcs = ["type_traits.cpp"],
    hdrs = ["type_traits.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//score/serialization:__pkg__"],
    deps = ["//score/language/futurecpp"],
)

cc_library(
    name = "details",
    srcs = [
        "serialization_visitor.cpp",
        "visit.cpp",
    ],
    hdrs = [
        "serialization_visitor.h",
        "visit.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//score/serialization:__pkg__"],
    deps = [
        "//score/language/futurecpp",
        "//score/serialization:strategy_interface",
        "//score/static_reflection_with_serialization/visitor",
    ],
)
