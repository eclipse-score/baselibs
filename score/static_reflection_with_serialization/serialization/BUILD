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
    name = "serialization",
    hdrs = [
        "include/serialization/for_logging.h",
        "include/serialization/skip_deserialize.h",
        "include/serialization/visit_serialize.h",
        "include/serialization/visit_size.h",
        "include/serialization/visit_type_traits.h",
    ],
    features = _compiler_warning_features,
    include_prefix = "static_reflection_with_serialization",
    strip_include_prefix = "include",
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/static_reflection_with_serialization/visitor",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":serializer_ut",
        ":size_visitor_ut",
        ":visitor_type_traits_ut",
    ],
    visibility = [
        # "@ddad@score_baselibs//score/mw/log/test:__pkg__",
    ],
)

cc_test(
    name = "serializer_ut",
    srcs = [
        "test/ut/test_serializer_visitor.cpp",
        "test/ut/test_skip_deserialize.cpp",
        "test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/mw/log/test:__pkg__",
        # "@ddad@score_baselibs//score/mw/log/test:__pkg__",
    ],
    deps = [
        ":serialization",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "size_visitor_ut",
    srcs = [
        "test/ut/test_size_visitor.cpp",
        "test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features,
    tags = ["unit"],
    visibility = [
        "@score_baselibs//score/mw/log/test:__pkg__",
        # "@ddad@score_baselibs//score/mw/log/test:__pkg__",
    ],
    deps = [
        ":serialization",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "visitor_type_traits_ut",
    srcs = [
        "test/ut/test_visitor_type_traits.cpp",
        "test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features,
    tags = ["unit"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":serialization",
        "@googletest//:gtest_main",
    ],
)
