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

load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

string_flag(
    name = "safety_level",
    build_setting_default = "asil_b",
    values = [
        "asil_b",
        "qm",
    ],
    visibility = [":__subpackages__"],
)

config_setting(
    name = "safety_level_qm",
    flag_values = {":safety_level": "qm"},
    visibility = [":__subpackages__"],
)

config_setting(
    name = "safety_level_asil_b",
    flag_values = {":safety_level": "asil_b"},
    visibility = [":__subpackages__"],
)
