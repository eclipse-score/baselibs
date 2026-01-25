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
load("@score_baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

test_suite(
    name = "unit_tests",
    tests = [
        "@score_baselibs//score/network/test:unit_tests",
    ],
    visibility = ["//platform/aas/lib:__pkg__"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    tags = ["manual"],
    test_suites = ["@score_baselibs//score/network/test:unit_tests_qnx"],
    visibility = ["//platform/aas/lib:__pkg__"],
)

cc_library(
    name = "ipv4_address",
    srcs = ["ipv4_address.cpp"],
    hdrs = ["ipv4_address.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "udp_socket",
    srcs = ["udp_socket.cpp"],
    hdrs = ["udp_socket.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    # The visibility is not public because it is still work in progress and
    # not ready to be used.
    visibility = [":__subpackages__"],
    deps = [
        ":ipv4_address",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:unistd",
    ],
)

cc_library(
    name = "vlan",
    srcs = ["vlan.cpp"],
    hdrs = ["vlan.h"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:version",
    ],
)

cc_library(
    name = "netutils",
    srcs = [
        "netutils.cpp",
        "netutils_impl.cpp",
        "netutils_impl.h",
    ] + select({
        "@platforms//os:linux": ["netutils_impl_linux.cpp"],
        "@platforms//os:qnx": ["netutils_impl_qnx.cpp"],
        "//conditions:default": ["netutils_impl_linux.cpp"],
    }),
    hdrs = ["netutils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ipv4_address",
        "@score_baselibs//score/mw/log:frontend",
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os:ifaddrs",
        "@score_baselibs//score/os:ioctl",
        "@score_baselibs//score/os:object_seam",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:unistd",
    ],
)

cc_library(
    name = "socket_interface",
    hdrs = ["i_socket.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/result",
    ],
)
