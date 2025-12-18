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

load("@safe_posix_platform//platform/aas/bazel/generators:aragen_tree.bzl", "cc_mw_com_library", "mw_artifacts")
load("@safe_posix_platform//platform/aas/bazel/generators:franca2arxml.bzl", "franca2arxml_converter")

def cc_middleware_com_frontend(
        name,
        srcs,
        target_compatible_with = [],
        **kwargs):
    """
    Marco to generate LoLa-Frontend from Franca-Files.

    This macro reduces duplication in Bazel Rules. It takes Franca as input and generates a cc_library that
    applications can link and build against. It is exclusively used to generate a LoLa (mw::com) frontend.
    """

    franca2arxml_converter(
        name = "{}_arxml".format(name),
        srcs = srcs,
        design_input_only = True,
        target_compatible_with = target_compatible_with,
    )

    mw_artifacts(
        name = "{}_mw_artifacts".format(name),
        arxmls = [
            ":{}_arxml".format(name),
            "@safe_posix_platform//platform/aas/meta/datatypes:stdtypes",
        ],
        target_compatible_with = target_compatible_with,
    )

    cc_mw_com_library(
        name = name,
        mw_artifacts = ":{}_mw_artifacts".format(name),
        target_compatible_with = target_compatible_with,
        **kwargs
    )
