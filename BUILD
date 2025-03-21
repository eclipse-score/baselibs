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
load("@aspect_rules_lint//format:defs.bzl", "format_multirun", "format_test")
load("@score_cr_checker//:cr_checker.bzl", "copyright_checker")

# TODO: Add all files/directories in `src` attribute that needs to be checked
copyright_checker(
    name = "copyright",
    srcs = [
        "//:BUILD",
        "//:MODULE.bazel",
    ],
    config = "@score_cr_checker//resources:config",
    template = "@score_cr_checker//resources:templates",
    visibility = ["//visibility:public"],
)

format_multirun(
    name = "format.fix",
    python = "@aspect_rules_lint//format:ruff",
    starlark = "@buildifier_prebuilt//:buildifier",
    visibility = [
        "//visibility:public",
    ],
    yaml = "@aspect_rules_lint//format:yamlfmt",
)

format_test(
    name = "format.check",
    no_sandbox = True,
    python = "@aspect_rules_lint//format:ruff",
    starlark = "@buildifier_prebuilt//:buildifier",
    visibility = [
        "//visibility:public",
    ],
    workspace = "//:MODULE.bazel",
    yaml = "@aspect_rules_lint//format:yamlfmt",
)
