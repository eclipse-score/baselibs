# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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

load("@rules_cc//cc:defs.bzl", "cc_test")
load("//third_party/openssl:openssl_transition.bzl", "openssl_library")

openssl_library(
    name = "openssl_default",
    dep = "@openssl//:crypto",
)

alias(
    name = "openssl",
    actual = select({
        "@platforms//os:qnx": "//third_party/qnx:openssl",
        "//conditions:default": ":openssl_default",
    }),
    visibility = ["//visibility:public"],
)

# Verify we have modeled all dynamic dependencies for this library correctly.
# This test will fail with RBE, when not all dependencies are modeled.
# Locally this may work, as the host system may contain the dependencies.
cc_test(
    name = "test_openssl",
    deps = [
        ":openssl",
        "@googletest//:gtest_main",
    ],
)
