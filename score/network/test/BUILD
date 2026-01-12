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
        ":ipv4_address_test",
        ":netutils_linux_test",
        ":udp_socket_test",
    ],
    visibility = ["@score_baselibs//score/network:__pkg__"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":ipv4_address_test",
        ":netutils_qnx_test",
        ":udp_socket_test",
        ":vlan_test",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "ipv4_address_test",
    srcs = ["ipv4_address_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/network:ipv4_address",
    ],
)

cc_test(
    name = "udp_socket_test",
    srcs = ["udp_socket_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/network:udp_socket",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
    ],
)

cc_test(
    name = "vlan_test",
    srcs = ["vlan_test.cpp"],
    features = [
        "treat_warnings_as_errors",
        "additional_warnings",
        "strict_warnings",
    ],
    tags = ["unit"],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/network:vlan",
        "@score_baselibs//score/os/mocklib:socket_mock",
    ],
)

cc_test(
    name = "netutils_qnx_test",
    srcs = [
        "netutils_qnx_test.cpp",
        "netutils_test.cpp",
    ],
    tags = ["manual"],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/network:netutils",
        "@score_baselibs//score/os/mocklib:ifaddrs_mock",
        "@score_baselibs//score/os/mocklib:ioctl_mock",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
    ],
)

cc_test(
    name = "netutils_linux_test",
    srcs = [
        "netutils_linux_test.cpp",
        "netutils_test.cpp",
    ],
    tags = ["unit"],
    target_compatible_with = ["@platforms//os:linux"],
    deps = [
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log:backend_stub_testutil",
        "@score_baselibs//score/network:netutils",
        "@score_baselibs//score/os/mocklib:ifaddrs_mock",
        "@score_baselibs//score/os/mocklib:ioctl_mock",
        "@score_baselibs//score/os/mocklib:socket_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
    ],
)
