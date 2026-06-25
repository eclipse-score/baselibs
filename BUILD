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

load("@score_bazel_tools_cc//quality:defs.bzl", "clang_format_config", "quality_clang_tidy_config")
load("@score_docs_as_code//:docs.bzl", "architecture_checklist", "assumptions_of_use", "component_architecture", "component_requirements", "docs", "feature_architecture", "feature_requirements", "filtered_needs_json", "requirements_checklist", "sphinx_needs_to_md", "sphinx_needs_to_trlc")
load("@score_tooling//:defs.bzl", "copyright_checker", "dash_license_checker", "rust_coverage_report", "use_format_targets")
load("//:project_config.bzl", "PROJECT_CONFIG")
load(":qemu.bzl", "qemu_aarch64")

docs(
    data = [
        "@score_platform//:needs_json",
        "@score_process//:needs_json",
    ],
    source_dir = "docs",
)

filtered_needs_json(
    name = "my_feat_reqs",
    src = "//:needs_json",
    types = ["comp_req", "comp_arc_sta"],
    # names = ["baselibs"],
    visibility = ["//visibility:public"],
)

sphinx_needs_to_md(
    name = "my_feat_reqs_doc",
    src = ":my_feat_reqs",
    title = "Baselibs feature architecture needs",
    visibility = ["//visibility:public"],
)

sphinx_needs_to_trlc(
    name = "my_feat_reqs_trlc",
    src = ":my_feat_reqs",
    package = "BaselibsNeeds",
    visibility = ["//visibility:public"],
)

feature_architecture(
    name = "baselibs_feature_arch",
    feature = "baselibs",
    visibility = ["//visibility:public"],
)

# Validate the baselibs feature architecture against the reviewed checklist
# (arch_chklst__baselibs__feat_arc). `bazel build` fails when the architecture
# output (or one of its transitive dependencies) drifts from the SHA256 pinned
# in the checklist need.
architecture_checklist(
    name = "baselibs_feat_arch_checklist",
    checklist_id = "arch_chklst__baselibs__feat_arc",
    deps = [":baselibs_feature_arch"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:public"],
)


# *****************************************************************************
# Example usage of the requirement extraction macros from score_docs_as_code.
#
# Each macro extracts one requirement type from //:needs_json and writes a
# "<name>.json" file. The optional "component" / "feature" argument is matched
# against the feature/component name encoded in each need ID (the
# "<type>__<name>__..." naming convention); if omitted, all requirements of
# that type are kept.
# *****************************************************************************

component_requirements(
    name = "bitmanipulation_comp_reqs",  # only the "bitmanipulation" component
    component = "bitmanipulation",
    visibility = ["//visibility:public"],
)

# Validate the bitmanipulation component requirements against the reviewed
# checklist (req_chklst__bitmanipulation__comp_req). `bazel build` fails when the
# requirements output drifts from the SHA256 pinned in the checklist need.
requirements_checklist(
    name = "bitmanipulation_req_checklist",
    checklist_id = "req_chklst__bitmanipulation__comp_req",
    deps = [":bitmanipulation_comp_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:public"],
)

component_architecture(
    name = "bitmanipulation_comp_arch",  # only the "bitmanipulation" component
    component = "bitmanipulation",
    visibility = ["//visibility:public"],
)

# Validate the bitmanipulation component architecture against the reviewed
# checklist (arch_chklst__bitmanipulation__comp_arc). `bazel build` fails when the
# architecture output drifts from the SHA256 pinned in the checklist need.
architecture_checklist(
    name = "bitmanipulation_arch_checklist",
    checklist_id = "arch_chklst__bitmanipulation__comp_arc",
    deps = [":bitmanipulation_comp_arch"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:public"],
)

copyright_checker(
    name = "copyright",
    srcs = [
        ".github",
        "bazel",
        "docs",
        "examples",
        "score",
        "src",
        "third_party",
        "//:BUILD",
        "//:MODULE.bazel",
        "//:qemu.bzl",
    ],
    config = "@score_tooling//cr_checker/resources:config",
    exclusion = "//:cr_checker_exclusion",
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
        "ferrocene-coverage",
    ],
    query = 'kind("rust_test", //src/...) except //src/log/score_log_fmt_macro:tests',
    visibility = ["//visibility:public"],
)

alias(
    name = "rust_coverage_report",
    actual = ":rust_coverage",
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
