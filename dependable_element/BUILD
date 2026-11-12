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

load(
    "@score_tooling//bazel/rules/rules_score:rules_score.bzl",
    "architectural_design",
    "component",
    "component_requirements",
    "dependable_element",
    "feature_requirements",
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
    static = ["static_design.puml"],
)

dependable_element(
    name = "baselibs",
    architectural_design = [":architectural_design"],
    components = [":bitmanipulation"],
    dependability_analysis = [],
    # Documented by //:docs, which imports :baselibs_needs.json.
    docs = False,
    integrity_level = "B",
    # Traceability gaps are reported as warnings while the prototype is incomplete.
    maturity = "development",
    requirements = [":feature_requirements"],
    tests = [],
    visibility = ["//:__pkg__"],
)
