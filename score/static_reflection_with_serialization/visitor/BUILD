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
_compiler_warning_features = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

cc_library(
    name = "visitor",
    hdrs = [
        "include/visitor/visit.h",
        "include/visitor/visit_as_struct.h",
    ],
    features = _compiler_warning_features,
    include_prefix = "static_reflection_with_serialization",
    strip_include_prefix = "include",
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    # No dependencies except for STL shall be used
    deps = [],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":visitor_ut",
    ],
    visibility = [
        # "@ddad@score_baselibs//score/mw/log/test:__pkg__",
    ],
)

cc_test(
    name = "visitor_ut",
    srcs = [
        "test/ut/test_detail.cpp",
        "test/ut/test_struct_visitor.cpp",
        "test/ut/test_visitor.cpp",
    ],
    features = _compiler_warning_features,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/mw/log/test:__pkg__",
        # "@ddad@score_baselibs//score/mw/log/test:__pkg__",
    ],
    deps = [
        ":visitor",
        "@googletest//:gtest_main",
    ],
)
