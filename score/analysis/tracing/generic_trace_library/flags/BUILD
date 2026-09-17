# *******************************************************************************
# Copyright (c) 2024 Contributors to the Eclipse Foundation
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
load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "enable_typed_memory",
    build_setting_default = False,
    visibility = [
        "//visibility:public",
    ],
)

# This config_setting is not used in this repository and should be always False
config_setting(
    name = "require_typed_memory",
    constraint_values = [
        "@platforms//os:qnx",
    ],
    flag_values = {
        ":enable_typed_memory": "True",
    },
    visibility = ["//visibility:public"],
)
