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
    name = "sock_async",
    srcs = [
        "sock_async.cpp",
    ],
    hdrs = [
        "sock_async.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/concurrency:thread_pool",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/network/sock_async:impl",
        "@score_baselibs//score/network/sock_async:net_endpoint",
        "@score_baselibs//score/os:socket",
    ],
)

cc_library(
    name = "net_endpoint",
    srcs = ["net_endpoint.cpp"],
    hdrs = ["net_endpoint.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/network:ipv4_address",
    ],
)

cc_library(
    name = "socket_factory",
    srcs = [
        "sock_factory.cpp",
    ],
    hdrs = [
        "sock_factory.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/network:ipv4_address",
        "@score_baselibs//score/network/sock_async:socket_ctrl",
        "@score_baselibs//score/network/sock_async_raw:socket_raw",
        "@score_baselibs//score/network/sock_async_tcp:socket_tcp",
        "@score_baselibs//score/network/sock_async_udp:socket_udp",
    ],
)

cc_library(
    name = "socket_ctrl",
    srcs = ["sock_ctrl.cpp"],
    hdrs = ["sock_ctrl.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],  # platform_only
    deps = [
        "@score_baselibs//score/concurrency:thread_pool",
        "@score_baselibs//score/network/sock_async",
        "@score_baselibs//score/os:sys_poll",
    ],
)

cc_library(
    name = "impl",
    srcs = [
        "socket.cpp",
        "socket.h",
    ],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/network:socket_interface",
        "@score_baselibs//score/network/sock_async:net_endpoint",
        "@score_baselibs//score/os:socket",
    ],
)

cc_test(
    name = "sock_async_test",
    srcs = ["sock_async_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    flaky = True,
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/concurrency:thread_pool",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/network/sock_async",
        "@score_baselibs//score/network/sock_async:socket_factory",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/mocklib:sys_poll_mock",
    ],
)

cc_test(
    name = "socket_test",
    srcs = ["socket_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    visibility = ["@score_baselibs//score/network/sock_async:__pkg__"],
    deps = [
        ":impl",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/os/mocklib:socket_mock",
    ],
)
