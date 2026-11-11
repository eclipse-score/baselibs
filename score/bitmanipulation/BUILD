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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_docs_as_code//:docs.bzl", "docs_bundle")
load(
    "@score_tooling//bazel/rules/rules_score:rules_score.bzl",
    "component",
    "component_requirements",
    "unit",
    "unit_design",
)

cc_test(
    name = "bit_manipulation_test",
    srcs = ["bit_manipulation_test.cpp"],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":bitmanipulation",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "bitmask_operators_test",
    srcs = ["bitmask_operators_test.cpp"],
    features = [
        "aborts_upon_exception",
        "strict_warnings",
    ],
    tags = ["unit"],
    deps = [
        ":bitmask_operators",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "bitmanipulation",
    srcs = ["bit_manipulation.cpp"],
    hdrs = ["bit_manipulation.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

cc_library(
    name = "bitmask_operators",
    srcs = ["bitmask_operators.cpp"],
    hdrs = ["bitmask_operators.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

docs_bundle(
    name = "docs",
    # Lets the bundle export its own Needs, which feed component_requirements below.
    external_needs = [
        "@score_platform//:needs_json_file",
        "@score_process_description//:needs_json_file",
    ],
    primary_need_id = "comp__baselibs_bit_manipulation",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)

# Prototype: rules_score dependable element with requirements from Sphinx-Needs,
# see //dependable_element.
component_requirements(
    name = "component_requirements",
    comp_id = "comp__baselibs_bit_manipulation",
    needs = [":docs"],
)

unit_design(
    name = "bit_manipulation_design",
    static = ["design/bit_manipulation.puml"],
)

unit(
    name = "bit_manipulation_unit",
    design_name = "bit_manipulation",
    tests = [":bit_manipulation_test"],
    unit_design = [":bit_manipulation_design"],
    implementation = [":bitmanipulation"],
)

unit_design(
    name = "bitmask_operators_design",
    static = ["design/bitmask_operators.puml"],
)

unit(
    name = "bitmask_operators_unit",
    design_name = "bitmask_operators",
    tests = [":bitmask_operators_test"],
    unit_design = [":bitmask_operators_design"],
    implementation = [":bitmask_operators"],
)

component(
    name = "component",
    components = [
        ":bit_manipulation_unit",
        ":bitmask_operators_unit",
    ],
    design_name = "bitmanipulation",
    requirements = [
        ":component_requirements",
        "//dependable_element:feature_requirements",
    ],
    tests = [],
    visibility = ["//dependable_element:__pkg__"],
)
