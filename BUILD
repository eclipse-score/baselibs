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

load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")
load("@score_bazel_tools_cc//quality:defs.bzl", "quality_clang_tidy_config")
load("@score_docs_as_code//:docs.bzl", "docs")
load("@score_tooling//:defs.bzl", "copyright_checker", "dash_license_checker")
load("@score_tooling//third_party/format:macros.bzl", "use_format_targets")
load("//:project_config.bzl", "PROJECT_CONFIG")
load(":qemu.bzl", "qemu_aarch64")

docs(
    bundles = [
        {
            "bundle": "//score/bitmanipulation:docs",
            "mount_at": "baselibs/components/bitmanipulation",
        },
        {
            "bundle": "//score/concurrency:docs",
            "mount_at": "baselibs/components/concurrency",
        },
        {
            "bundle": "//score/containers:docs",
            "mount_at": "baselibs/components/containers",
        },
        {
            "bundle": "//score/containers_rust:docs",
            "mount_at": "baselibs/components/containers_rust",
        },
        {
            "bundle": "//score/filesystem:docs",
            "mount_at": "baselibs/components/filesystem",
        },
        {
            "bundle": "//score/flatbuffers:docs",
            "mount_at": "baselibs/components/flatbuffers",
        },
        {
            "bundle": "//score/hash:docs",
            "mount_at": "baselibs/components/hash",
        },
        {
            "bundle": "//score/json:docs",
            "mount_at": "baselibs/components/json",
        },
        {
            "bundle": "//score/language:docs",
            "mount_at": "baselibs/components/language",
        },
        {
            "bundle": "//score/language/futurecpp:docs",
            "mount_at": "baselibs/components/language/futurecpp",
        },
        {
            "bundle": "//score/language/safecpp:docs",
            "mount_at": "baselibs/components/language/safecpp",
        },
        {
            "bundle": "//score/memory:docs",
            "mount_at": "baselibs/components/memory",
        },
        {
            "bundle": "//score/mw/log:docs",
            "mount_at": "baselibs/components/mw_log",
        },
        {
            "bundle": "//score/os:docs",
            "mount_at": "baselibs/components/os",
        },
        {
            "bundle": "//score/result:docs",
            "mount_at": "baselibs/components/result",
        },
        {
            "bundle": "//score/static_reflection_with_serialization:docs",
            "mount_at": "baselibs/components/static_reflection_with_serialization",
        },
        {
            "bundle": "//score/utils:docs",
            "mount_at": "baselibs/components/utils",
        },
    ],
    external_needs = [
        "@score_platform//:needs_json_file",
        "@score_process_description//:needs_json_file",
    ],
    source_dir = "docs",
)

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

# Generate `rust_project.json`.
# Required for `rust-analyzer` support.
alias(
    name = "generate_rust_project",
    actual = "@rules_rust//tools/rust_analyzer:gen_rust_project",
    target_compatible_with = ["@platforms//os:linux"],
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

# Needed for Dash tool to check python dependency licenses.
# This is a workaround to filter out local packages from the Cargo.lock file.
# The tool is intended for third-party content.
genrule(
    name = "filtered_cargo_lock",
    srcs = ["Cargo.lock"],
    outs = ["Cargo.lock.filtered"],
    cmd = """
    awk '
    BEGIN { skip = 0; data = "" }
    /^\\[\\[package\\]\\]/ {
        if (data != "" && !skip) print data;
        skip = 1;
        data = $$0;
        next;
    }
    data != "" { data = data "\\n" $$0 }
    # any package that has a "source = " line will not be skipped.
    /^source = / { skip = 0 }
    END { if (data != "" && !skip) print data }
    ' $(location Cargo.lock) > $@
    """,
)

dash_license_checker(
    src = ":filtered_cargo_lock",
    file_type = "",  # let it auto-detect based on project_config
    project_config = PROJECT_CONFIG,
    visibility = ["//visibility:public"],
)

# TODO: rust_coverage_report was removed by score_tooling >= 2.1.0
# //:rust_coverage and //:rust_coverage_report are gone until the repo migrates
# to the new score_coverage_scope/score_coverage_reporter LLVM pipeline
# https://github.com/eclipse-score/baselibs/issues/512

qemu_aarch64()

use_format_targets(languages = [
    "python",
    "rust",
    "starlark",
    "yaml",
    "cpp",
])

filegroup(
    name = "clang_tidy_config_files",
    srcs = [
        ".clang-tidy-minimal",
    ],
    visibility = ["//visibility:public"],
)

quality_clang_tidy_config(
    name = "clang_tidy_config",
    additional_flags = [],
    clang_tidy_binary = "@llvm_toolchain//:clang-tidy",
    default_feature = "strict",
    dependency_attributes = [
        "deps",
        "srcs",
    ],
    excludes = [],
    feature_mapping = {
        "//:.clang-tidy-minimal": "strict",
    },
    target_types = [
        "cc_library",
    ],
    unsupported_flags = [],
    visibility = ["//visibility:public"],
)
