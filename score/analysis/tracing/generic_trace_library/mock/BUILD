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
    name = "trace_library_mock",
    srcs = ["trace_library_mock.cpp"],
    hdrs = ["trace_library_mock.h"],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/stub_implementation",
        "@googletest//:gtest_main",
    ],
)
