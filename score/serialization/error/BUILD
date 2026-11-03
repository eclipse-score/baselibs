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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "error",
    srcs = ["serialization_error.cpp"],
    hdrs = ["serialization_error.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["FFI"],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "//score/language/futurecpp",
        "//score/result",
    ],
)

cc_test(
    name = "unit_test",
    srcs = ["serialization_error_test.cpp"],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    tags = ["unit"],
    visibility = ["//score/serialization:__pkg__"],
    deps = [
        ":error",
        "@googletest//:gtest_main",
    ],
)
