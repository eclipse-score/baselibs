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

# Tooling entrypoints backed by @score_tooling and @hedron_compile_commands.
# Kept out of the root BUILD so that package stays free of these dev dependencies.
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")
load("@score_tooling//:defs.bzl", "copyright_checker", "dash_license_checker")
load("@score_tooling//third_party/format:macros.bzl", "use_format_targets")
load("//:project_config.bzl", "PROJECT_CONFIG")

# Generate `compile_commands.json`.
# Required for `clangd` support.
refresh_compile_commands(
    name = "generate_compile_commands",
    exclude_external_sources = True,
    target_compatible_with = ["@platforms//os:linux"],
    targets = {
        "//...": "",
    },
)

copyright_checker(
    name = "copyright",
    srcs = [
        ".github",
        "BUILD",
        "MODULE.bazel",
        "bazel",
        "docs",
        "examples",
        "qemu.bzl",
        "score",
        "third_party",
    ],
    config = "@score_tooling//cr_checker/resources:config",
    exclusion = "//:cr_checker_exclusion",
    extensions = [
        "bazel",
        "BUILD",
        "bzl",
        "c",
        "cpp",
        "h",
        "hpp",
        "ini",
        "py",
        "rs",
        "rst",
        "sh",
        "yaml",
        "yml",
    ],
    template = "@score_tooling//cr_checker/resources:templates",
    visibility = ["//visibility:public"],
)

dash_license_checker(
    src = "//:filtered_cargo_lock",
    file_type = "",  # let it auto-detect based on project_config
    project_config = PROJECT_CONFIG,
    visibility = ["//visibility:public"],
)

use_format_targets(languages = [
    "python",
    "rust",
    "starlark",
    "yaml",
    "cpp",
])
