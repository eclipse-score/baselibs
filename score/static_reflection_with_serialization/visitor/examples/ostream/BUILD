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
cc_library(
    name = "visitor_examples",
    hdrs = [
        "include/visitor/visit_ostream.h",
    ],
    strip_include_prefix = "include",
    tags = ["aas"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
    ],
)

cc_test(
    name = "visitorUT",
    srcs = [
        "test/ut/test_ostream_visitor.cpp",
    ],
    copts = [
        "-Werror=pedantic",
        "-Werror=extra",
    ],
    deps = [
        ":visitor_examples",
        "@googletest//:gtest_main",
    ],
)
