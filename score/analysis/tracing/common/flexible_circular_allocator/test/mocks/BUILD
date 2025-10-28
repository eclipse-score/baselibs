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
    name = "flexible_circular_allocator_mock",
    testonly = True,
    hdrs = [
        "flexible_circular_allocator_mock.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        "@score_baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator:flexible_circular_allocator_interface",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "flexible_circular_allocator_factory_mock",
    testonly = True,
    hdrs = [
        "flexible_circular_allocator_factory_mock.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
    ],
    visibility = [
        "@score_baselibs//score/analysis:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/analysis/tracing/common/flexible_circular_allocator:flexible_circular_allocator_factory_interface",
        "@googletest//:gtest_main",
    ],
)
