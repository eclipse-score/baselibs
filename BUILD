# *******************************************************************************
# Copyright (c) 2024 Contributors to the Eclipse Foundation
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

load("@score_bazel_tools_cc//quality:defs.bzl", "clang_format_config", "quality_clang_tidy_config")
load("@score_docs_as_code//:docs.bzl", "docs")
load("@score_tooling//:defs.bzl", "copyright_checker", "use_format_targets")
load(":qemu.bzl", "qemu_aarch64")

docs(
    data = [
        "@score_process//:needs_json",
    ],
    source_dir = "docs",
)

copyright_checker(
    name = "copyright",
    srcs = [
        ".github",
        "bazel",
        "docs",
        "score",
        "//:BUILD",
        "//:MODULE.bazel",
    ],
    config = "@score_tooling//cr_checker/resources:config",
    exclusion = "//:cr_checker_exclusion",
    template = "@score_tooling//cr_checker/resources:templates",
    visibility = ["//visibility:public"],
)

qemu_aarch64()

use_format_targets()

clang_format_config(
    name = "clang_format_config",
    config_file = "//:.clang-format",
    target_types = [
        "cc_binary",
        "cc_library",
        "cc_test",
    ],
    visibility = ["//visibility:public"],
)

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
