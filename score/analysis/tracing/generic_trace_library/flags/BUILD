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

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

config_setting(
    name = "require_typed_memory",  #This is a stub and should always be false
    constraint_values = [
        "@platforms//os:qnx",
        "@platforms//os:linux",  # Contradicts qnx — ensures this config_setting never matches (stub)
    ],
    visibility = ["//visibility:public"],
)
