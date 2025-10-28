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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_test(
    name = "json_object",
    testonly = True,
    srcs = [
        "json_object.cpp",
    ],
    data = [
        "example.json",
        "logging.json",
    ],
    features = COMPILER_WARNING_FEATURES,
    # This is not a unit test, but with this tag we ensure the program is checked in the CI to prevent breaking changes.
    # It will not be accounted for unit test coverage as we do not add it here: `@score_baselibs//score/json:unit_tests`
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/json",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/mw/log",
    ],
)

cc_test(
    name = "json_list",
    testonly = True,
    srcs = [
        "json_list.cpp",
    ],
    data = [
        "example.json",
        "logging.json",
    ],
    features = COMPILER_WARNING_FEATURES,
    # This is not a unit test, but with this tag we ensure the program is checked in the CI to prevent breaking changes.
    # It will not be accounted for unit test coverage as we do not add it here: `@score_baselibs//score/json:unit_tests`
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/json",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/mw/log",
    ],
)

cc_test(
    name = "json_buffer",
    testonly = True,
    srcs = [
        "json_buffer.cpp",
    ],
    data = [
        "logging.json",
    ],
    features = COMPILER_WARNING_FEATURES,
    # This is not a unit test, but with this tag we ensure the program is checked in the CI to prevent breaking changes.
    # It will not be accounted for unit test coverage as we do not add it here: `@score_baselibs//score/json:unit_tests`
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/json",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/mw/log",
    ],
)
