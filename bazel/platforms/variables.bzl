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

xpad_constraints = [
    ":xpad",
    "//bazel/platforms/libraries:systemd",
]

xpad_dnv_constraints = xpad_constraints + [
    ":dnv",
]

wasm32_constraints = [
    "@platforms//cpu:wasm32",
    "@platforms//os:none",
]

host_constraints = [
    ":host",
]

host_gcc8_constraints = host_constraints + [
    ":gcc",
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]

host_gcc9_constraints = host_constraints + [
    "@bazel_tools//tools/cpp:gcc",
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
    "//bazel/platforms/libraries:systemd",
]

host_gcc12_constraints = host_constraints + [
    ":gcc",
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]
