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

# Prototype: baselibs as dependable element of rules_score with requirements from
# Sphinx-Needs (lobster_sphinx_needs) instead of TRLC. Currently one component.
# Kept out of //score/...: score_tooling is a dev dependency and //score/... is built for QNX.

load("@bazel_skylib//rules:diff_test.bzl", "diff_test")
load(
    "@score_tooling//bazel/rules/rules_score:rules_score.bzl",
    "architectural_design",
    "component",
    "component_requirements",
    "dependability_analysis",
    "dependable_element",
    "failure_modes",
    "fault_trees",
    "feature_requirements",
    "safety_analysis",
    "unit",
    "unit_design",
)

feature_requirements(
    name = "feature_requirements",
    feat_id = "feat__baselibs",
    needs = ["@score_platform//:needs_json_file"],
)

component_requirements(
    name = "bitmanipulation_requirements",
    comp_id = "comp__baselibs_bit_manipulation",
    needs = ["//score/bitmanipulation:docs"],
)

unit_design(
    name = "bit_manipulation_design",
    static = ["//score/bitmanipulation:docs/architecture/bit_manipulation.puml"],
)

unit(
    name = "bit_manipulation_unit",
    design_name = "bit_manipulation",
    tests = ["//score/bitmanipulation:bit_manipulation_test"],
    unit_design = [":bit_manipulation_design"],
    implementation = ["//score/bitmanipulation"],
)

unit_design(
    name = "bitmask_operators_design",
    static = ["//score/bitmanipulation:docs/architecture/bitmask_operators.puml"],
)

unit(
    name = "bitmask_operators_unit",
    design_name = "bitmask_operators",
    tests = ["//score/bitmanipulation:bitmask_operators_test"],
    unit_design = [":bitmask_operators_design"],
    implementation = ["//score/bitmanipulation:bitmask_operators"],
)

component(
    name = "bitmanipulation",
    components = [
        ":bit_manipulation_unit",
        ":bitmask_operators_unit",
    ],
    requirements = [
        ":bitmanipulation_requirements",
        ":feature_requirements",
    ],
    tests = [],
)

architectural_design(
    name = "architectural_design",
    dynamic = ["//score/bitmanipulation:docs/architecture/dynamic.puml"],
    # TRLC package of the generated ApiMethod records (public_api.trlc).
    package = "baselibs_api",
    public_api = ["//score/bitmanipulation:docs/architecture/public_api.puml"],
    static = ["//:docs/module/dependable_element.puml"],
)

# The FMEA stays Sphinx-Needs (comp_saf_fmea); fault trees reference its needs by ID.
failure_modes(
    name = "bitmanipulation_failure_modes",
    needs = ["//score/bitmanipulation:docs"],
    package = "BitmanipulationFmea",
    deps = [":architectural_design"],
)

fault_trees(
    name = "bitmanipulation_fault_trees",
    srcs = [
        "//score/bitmanipulation:docs/safety_analysis/fta_constraints.puml",
        "//score/bitmanipulation:docs/safety_analysis/fta_late_return.puml",
        "//score/bitmanipulation:docs/safety_analysis/fta_no_return.puml",
        "//score/bitmanipulation:docs/safety_analysis/fta_wrong_input.puml",
    ],
    fta_package = "BitmanipulationFta",
    deps = [":bitmanipulation_failure_modes"],
)

# The docs render the fault trees with a copy of the FTA macros (PlantUML resolves
# !include next to fmea.rst); it must not drift from score_tooling.
diff_test(
    name = "fta_metamodel_copy_test",
    file1 = "@score_tooling//plantuml:fta_metamodel",
    file2 = "//score/bitmanipulation:docs/safety_analysis/fta_metamodel.puml",
)

# Without TRLC assessments, each FMEA need rated sufficient assesses its root causes.
safety_analysis(
    name = "bitmanipulation_safety_analysis",
    failure_modes = [":bitmanipulation_failure_modes"],
    fault_trees = ":bitmanipulation_fault_trees",
    safety_measures = [":bitmanipulation_requirements"],
)

dependability_analysis(
    name = "bitmanipulation_dependability_analysis",
    arch_design = ":architectural_design",
    safety_analysis = [":bitmanipulation_safety_analysis"],
)

dependable_element(
    name = "baselibs",
    architectural_design = [":architectural_design"],
    components = [":bitmanipulation"],
    dependability_analysis = [":bitmanipulation_dependability_analysis"],
    # Documented by //:docs, which imports :baselibs_needs.json.
    docs = False,
    integrity_level = "B",
    # Traceability gaps are reported as warnings while the prototype is incomplete.
    maturity = "development",
    requirements = [":feature_requirements"],
    tests = [],
    visibility = ["//:__pkg__"],
)
