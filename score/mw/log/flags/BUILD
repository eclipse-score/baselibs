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

load("@bazel_skylib//lib:selects.bzl", "selects")
load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@rules_cc//cc:defs.bzl", "cc_library")

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

bool_flag(
    name = "use_dlt_trace",
    build_setting_default = False,
)

config_setting(
    name = "config_use_dlt_trace",
    flag_values = {
        ":use_dlt_trace": "True",
    },
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)

config_setting(
    name = "config_not_use_dlt_trace",
    flag_values = {
        ":use_dlt_trace": "False",
    },
)

selects.config_setting_group(
    name = "dlt_trace_enabled",
    match_all = [
        ":Remote_Logging",
        ":config_use_dlt_trace",
    ],
    visibility = [
        "@score_baselibs//score/mw/log:__subpackages__",
    ],
)

selects.config_setting_group(
    name = "datarouter_enabled",
    match_all = [
        ":Remote_Logging",
        ":config_not_use_dlt_trace",
    ],
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
