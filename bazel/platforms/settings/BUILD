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

package(default_visibility = ["//visibility:public"])

config_setting(
    name = "arm64-linux",
    constraint_values = [
        "@platforms//cpu:arm64",
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "arm64-qnx",
    constraint_values = [
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
    ],
)

config_setting(
    name = "arm64-qnx-hardware",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:hardware",
    ],
)

config_setting(
    name = "arm64-qnx-virtual",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:virtual",
    ],
)

config_setting(
    name = "arm64-qnx-qqvp",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:qqvp",
    ],
)

config_setting(
    name = "arm64-qnx-qemu",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:qemu",
    ],
)

config_setting(
    name = "arm64-qnx7-hardware",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:hardware",
        "@score_bazel_platforms//:qnx7_1",
    ],
)

config_setting(
    name = "arm64-qnx7-virtual",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:virtual",
        "@score_bazel_platforms//:qnx7_1",
    ],
)

config_setting(
    name = "arm64-qnx8-hardware",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:hardware",
        "@score_bazel_platforms//:qnx8_0",
    ],
)

config_setting(
    name = "arm64-qnx8-virtual",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:arm64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:virtual",
        "@score_bazel_platforms//:qnx8_0",
    ],
)

config_setting(
    name = "x86_64-linux",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "x86_64-qnx",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:qnx",
    ],
)

config_setting(
    name = "x86_64-qnx8-qemu",
    constraint_values = [
        "@swf_platforms//autosar:adaptive",
        "@platforms//cpu:x86_64",
        "@platforms//os:qnx",
        "//bazel/platforms/environment:qemu",
        "@score_bazel_platforms//:qnx8_0",
    ],
    visibility = [
        "//ecu/ipnext/tests:__subpackages__",
        "//ecu/reference/qnx_qemu/test:__subpackages__",
        "//ecu/xpad/xpad-shared/platform_tests:__subpackages__",
        "//ecu/xpad/xpad-shared/vendor_tests:__subpackages__",
        "//visibility:public",  # platform_only
        # "@ddad//ecu/ipnext/tests:__subpackages__",
        # "@ddad//ecu/xpad/xpad-shared/platform_tests:__subpackages__",
        # "@ddad//ecu/xpad/xpad-shared/vendor_tests:__subpackages__",
    ],
)
