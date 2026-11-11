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

load(
    "@score_tooling//bazel/rules/rules_score:rules_score.bzl",
    "architectural_design",
    "dependable_element",
    "feature_requirements",
)

feature_requirements(
    name = "feature_requirements",
    feat_id = "feat__baselibs",
    needs = ["@score_platform//:needs_json_file"],
    visibility = ["//score:__subpackages__"],
)

architectural_design(
    name = "architectural_design",
    static = ["static_design.puml"],
)

dependable_element(
    name = "baselibs",
    architectural_design = [":architectural_design"],
    components = ["//score/bitmanipulation:component"],
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
