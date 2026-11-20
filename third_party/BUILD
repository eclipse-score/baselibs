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

load("@bazel_skylib//lib:selects.bzl", "selects")

config_setting(
    name = "ebclfsa",
    constraint_values = [
        "@score_bazel_platforms//runtime_es:ebclfsa",
    ],
)

config_setting(
    name = "autosd10",
    constraint_values = [
        "@score_bazel_platforms//runtime_es:autosd10",
    ],
)

selects.config_setting_group(
    name = "use_system_libs",
    match_any = [
        ":ebclfsa",
        ":autosd10",
    ],
    visibility = ["//:__subpackages__"],
)
