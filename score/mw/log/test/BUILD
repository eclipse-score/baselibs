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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        "@score_baselibs//score/static_reflection_with_serialization/serialization:serializer_ut",
        "@score_baselibs//score/static_reflection_with_serialization/serialization:size_visitor_ut",
        "@score_baselibs//score/static_reflection_with_serialization/serialization:visitor_type_traits_ut",
        "@score_baselibs//score/static_reflection_with_serialization/visitor:visitor_ut",
        #"@score_baselibs//score/mw/log/test/log_stream_ara_core_types:log_stream_ara_core_types_ut",
    ],
    visibility = ["@score_baselibs//score/mw/log:__pkg__"],
)
