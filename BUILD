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
load("@score_docs_as_code//:docs.bzl", "architecture_checklist", "assumptions_of_use", "component_architecture", "component_requirements", "dfa_checklist", "docs", "feature_architecture", "feature_requirements", "filtered_needs_json", "fmea_checklist", "requirements_checklist", "score_component", "score_module", "sphinx_needs_to_md", "sphinx_needs_to_trlc", "verification_report")
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

# *****************************************************************************
# High-level wrappers: score_module / score_component.
#
# `score_component` bundles a component's requirement + architecture checklists.
# `score_module` bundles a feature's requirement + architecture checklists with
# all of its components. Building the module target also builds every component.
# *****************************************************************************

# --- bitmanipulation component ---------------------------------------------
component_requirements(
    name = "bitmanipulation_comp_reqs",
    src = "//:needs_json",
    component = "bitmanipulation",
    visibility = ["//visibility:private"],
)

requirements_checklist(
    name = "bitmanipulation_req_checklist",
    mod_insp_id = "mod_insp__bitmanipulation__comp_req",
    deps = [":bitmanipulation_comp_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

component_architecture(
    name = "bitmanipulation_comp_arch",
    src = "//:needs_json",
    component = "bitmanipulation",
    visibility = ["//visibility:private"],
)

architecture_checklist(
    name = "bitmanipulation_arch_checklist",
    mod_insp_id = "mod_insp__bitmanipulation__comp_arc",
    deps = [":bitmanipulation_comp_arch"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

dfa_checklist(
    name = "bitmanipulation_dfa_checklist",
    mod_insp_id = "mod_insp__bitmanipulation__comp_dfa",
    deps = [":bitmanipulation_comp_arch", ":bitmanipulation_comp_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

fmea_checklist(
    name = "bitmanipulation_fmea_checklist",
    mod_insp_id = "mod_insp__bitmanipulation__comp_fmea",
    deps = [":bitmanipulation_comp_arch", ":bitmanipulation_comp_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

score_component(
    name = "bitmanipulation",
    req_chklst = [":bitmanipulation_req_checklist"],
    arch_chklst = [":bitmanipulation_arch_checklist"],
    dfa = ":bitmanipulation_dfa_checklist",
    fmea = ":bitmanipulation_fmea_checklist",
    visibility = ["//visibility:private"],
)

# --- baselibs feature ------------------------------------------------------
feature_requirements(
    name = "baselibs_feature_reqs",
    src = "@score_platform//:needs_json",
    feature = "baselibs",
    visibility = ["//visibility:private"],
)

requirements_checklist(
    name = "baselibs_req_checklist",
    mod_insp_id = "mod_insp__baselibs__feat_req",
    deps = [":baselibs_feature_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

feature_architecture(
    name = "baselibs_feature_arch",
    src = "//:needs_json",
    feature = "baselibs",
    visibility = ["//visibility:private"],
)

architecture_checklist(
    name = "baselibs_arch_checklist",
    mod_insp_id = "mod_insp__baselibs__feat_arc",
    deps = [":baselibs_feature_arch"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

dfa_checklist(
    name = "baselibs_dfa_checklist",
    mod_insp_id = "mod_insp__baselibs__feat_dfa",
    deps = [":baselibs_feature_arch", ":baselibs_feature_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

fmea_checklist(
    name = "baselibs_fmea_checklist",
    mod_insp_id = "mod_insp__baselibs__feat_fmea",
    deps = [":baselibs_feature_arch", ":baselibs_feature_reqs"],
    extra_needs = ["@score_platform//:needs_json"],
    visibility = ["//visibility:private"],
)

# verification_report(
#     name = "baselibs_verification_report",
#     mod_ver_report_id = "mod_vrep__baselibs__module",
#     deps = [],
#     extra_needs = ["@score_platform//:needs_json"],
#     visibility = ["//visibility:private"],
# )

score_module(
    name = "baselibs",
    docs = ":docs",
    req_chklst = [":baselibs_req_checklist"],
    arch_chklst = [":baselibs_arch_checklist"],
    components = [":bitmanipulation"],
    fmea = ":baselibs_fmea_checklist",
    # verif_report = ":baselibs_verification_report",
    dfa = ":baselibs_dfa_checklist",
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
