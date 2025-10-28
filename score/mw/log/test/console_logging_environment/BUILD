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
# You can depend on this if you don't need further customization of the main() for gtest.
cc_library(
    name = "console_logging_environment",
    testonly = True,
    srcs = [
        "main.cpp",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":console_logging_environment_lib",
    ],
)

# You can depend on this if you need a customized main() for gtest.
cc_library(
    name = "console_logging_environment_lib",
    testonly = True,
    srcs = [
        "console_logging_environment.cpp",
    ],
    hdrs = [
        "console_logging_environment.h",
    ],
    deps = [
        "@score_baselibs//score/mw/log",
        "@googletest//:gtest",
    ],
)
