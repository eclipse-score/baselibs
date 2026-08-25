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
load("@score_docs_as_code//:bzl/docs_and_test.bzl", "docs_and_test")
load("@score_tooling//:defs.bzl", "copyright_checker", "dash_license_checker", "rust_coverage_report", "use_format_targets")
load("//:project_config.bzl", "PROJECT_CONFIG")
load(":qemu.bzl", "qemu_aarch64")

docs(
    external_needs = [
        "@score_platform//:needs_json_file",
        "@score_process_description//:needs_json_file",
    ],
    source_dir = "docs",
)

# Builds/serves the module verification report for a small, known-good subset
# of //score/... with coverage. The full //score/... tree currently has ~20
# pre-existing, unrelated build failures under bl-aarch64-linux (deprecated
# logging API under -Werror=deprecated-declarations, and a Rust nightly-only
# feature used by score_log_fmt), so this uses a reduced scope sufficient to
# validate the docs_and_test integration and coverage report rendering.
#
# Extra Bazel flags (e.g. --config=…) go on the command line after --, e.g.::
#
#     bazel run //:module_verification_report -- --test-flag=--config=bl-aarch64-linux
#
# See @score_docs_as_code//:bzl/docs_and_test.bzl for the underlying driver.
docs_and_test(
    name = "module_verification_report",
    test_targets = [
        "//score/bitmanipulation/...",
        "//score/flatbuffers/...",
        "//score/filesystem/...",
    ],
    docs_target = "//:docs",
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
        "bazel",
        "docs",
        "examples",
        "score",
        "third_party",
        "//:BUILD",
        "//:MODULE.bazel",
        "//:qemu.bzl",
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

rust_coverage_report(
    name = "rust_coverage",
    bazel_configs = [
        "bl-x86_64-linux",
        "ferrocene-coverage",
    ],
    query = 'kind("rust_test", //score/...) except //score/log_rust/score_log_fmt_macro:tests',
    visibility = ["//visibility:public"],
)

alias(
    name = "rust_coverage_report",
    actual = ":rust_coverage",
    visibility = ["//visibility:public"],
)

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
