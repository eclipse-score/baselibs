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
    name = "socket_tcp",
    srcs = ["socket_tcp.cpp"],
    hdrs = [
        "socket_tcp.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/network:socket_interface",
        "@score_baselibs//score/network/sock_async",
        "@score_baselibs//score/network/sock_async:socket_ctrl",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:unistd",
    ],
)

cc_test(
    name = "socket_tcp_test",
    srcs = ["socket_tcp_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/network/sock_async:__pkg__"],
    deps = [
        ":socket_tcp",
        "@googletest//:gtest_main",
        "@score_baselibs//score/concurrency:thread_pool",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/network/sock_async:socket_factory",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/mocklib:sys_poll_mock",
    ],
)
