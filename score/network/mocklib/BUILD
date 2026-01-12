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
    name = "udp_socket_mock",
    testonly = True,
    srcs = ["udp_socket_mock.cpp"],
    hdrs = ["udp_socket_mock.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/network:udp_socket",
    ],
)

cc_library(
    name = "netutils_mock",
    testonly = True,
    hdrs = ["netutilsmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@googletest//:gtest",
        "@score_baselibs//score/network:netutils",
    ],
)
