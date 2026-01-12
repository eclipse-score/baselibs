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

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "KRemote_Logging",
    build_setting_default = True,
)

config_setting(
    name = "Remote_Logging",
    flag_values = {
        ":KRemote_Logging": "True",
    },
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)

bool_flag(
    name = "KCustom_Logging",
    build_setting_default = False,
)

config_setting(
    name = "Custom_Logging",
    flag_values = {
        ":KCustom_Logging": "True",
    },
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)

cc_library(
    name = "unfilled",
)

label_flag(
    name = "custom_recorder_impl",
    build_setting_default = ":unfilled",
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)
