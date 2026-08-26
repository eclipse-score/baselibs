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
load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_docs_as_code//:docs.bzl", "docs_bundle")
load("@score_tooling//bazel/rules/rules_score:rules_score.bzl", "architectural_design", "assumptions_of_use", "component", "component_requirements", "dependable_element", "unit")

# Exposes the raw RST source containing the comp_req directives, e.g. for
# other consumers that don't need TrlcProviderInfo.
filegroup(
    name = "comp_req_bitmanipulation_rst",
    srcs = ["docs/requirements/index.rst"],
    visibility = ["//visibility:public"],
)

# Converts the comp_req directives to TRLC. derived_from references in the
# RST point to feat_req__baselibs__bitmanipulation, which lives in the
# feat_req_baselibs target of the local score checkout (see
# local_path_override for score_platform in //MODULE.bazel). ref_package
# must match the TRLC package name that target's RST conversion produces
# (default-derived from its "index.rst" filename stem: "Index"). Our own
# RST is also named "index.rst", so without an explicit `package` override
# our own package would also default to "Index" and collide (self-import)
# with the ref_package we need to import here. See //../tooling (local
# score_tooling checkout, local_path_override in //MODULE.bazel) for the
# `package` param added to component_requirements for this purpose.
component_requirements(
    name = "comp_req_bitmanipulation",
    srcs = ["docs/requirements/index.rst"],
    deps = ["@score_platform//docs/features/baselibs/requirements:feat_req_baselibs"],
    package = "CompReqBitmanipulation",
    ref_package = "Index",
    visibility = ["//visibility:public"],
)

# The same index.rst also already contains the aou_req directives for this
# component (Assumptions of Use section) - no separate file needed. None of
# them use :derived_from:/:satisfies:, so no ref_package is required here.
# requirements points at comp_req_bitmanipulation (component level, not
# feat_req_baselibs), matching the S-CORE convention that AoUs trace to/count
# towards Component Requirements, not Feature Requirements.
assumptions_of_use(
    name = "aous_bitmanipulation",
    srcs = ["docs/requirements/index.rst"],
    requirements = [":comp_req_bitmanipulation"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "bit_manipulation_test",
    srcs = ["bit_manipulation_test.cpp"],
    features = [
        "aborts_upon_exception",
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
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
        "treat_warnings_as_errors",
        "strict_warnings",
        "additional_warnings",
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

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":bit_manipulation_test",
        ":bitmask_operators_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)

# design/README.md explains that no detailed design is provided for this
# library (it just aggregates small, uncoupled utility functions), so both
# units are defined without unit_design content.
unit(
    name = "bit_manipulation",
    unit_design = [],
    implementation = [":bitmanipulation"],
    scope = ["//score/bitmanipulation:bitmanipulation"],
    tests = [":bit_manipulation_test"],
    visibility = ["//visibility:public"],
)

unit(
    name = "bitmask",
    unit_design = [],
    implementation = [":bitmask_operators"],
    scope = ["//score/bitmanipulation:bitmask_operators"],
    tests = [":bitmask_operators_test"],
    visibility = ["//visibility:public"],
)

component(
    name = "component_bitmanipulation",
    components = [
        ":bit_manipulation",
        ":bitmask",
    ],
    # feat_req_baselibs is listed alongside comp_req_bitmanipulation (not just
    # in comp_req_bitmanipulation's own `deps`) so its .lobster file is
    # available to the component rule's lobster report generation, which
    # otherwise can't resolve comp_req_bitmanipulation's derived_from
    # reference to it ("unknown tracing target"). See component()'s
    # docstring in //../tooling (local score_tooling checkout).
    requirements = [
        ":comp_req_bitmanipulation",
        "@score_platform//docs/features/baselibs/requirements:feat_req_baselibs",
    ],
    visibility = ["//visibility:public"],
)

# Static architecture diagram (SEooC -> component_bitmanipulation -> its 2
# units). Validated at build time against the actual unit()/component()
# structure below; the build fails if they drift apart.
architectural_design(
    name = "arch_design_bitmanipulation",
    static = ["docs/architecture/static_design.puml"],
    visibility = ["//visibility:public"],
)

# Complete SEooC for bitmanipulation, assembling requirements, AoUs,
# architecture and the implementing component into a single certifiable
# dependable element.
#
# requirements points at feat_req_baselibs, not comp_req_bitmanipulation:
# dependable_element.requirements only accepts targets providing
# FeatureRequirementsInfo/AssumedSystemRequirementsInfo. comp_req_bitmanipulation
# (ComponentRequirementsInfo) is already pulled in transitively via
# `components` -> component_bitmanipulation's own `requirements` attribute.
# feat_req_baselibs contains all 14 baselibs feature requirements (not just
# bitmanipulation's) - see Notes.md for why this is used pragmatically for now
# instead of a dedicated, bitmanipulation-only feature requirements target.
#
# dependability_analysis/tests are empty for now: docs/safety_analysis/*.rst
# has no fmea()/dfa() Bazel targets yet (those rules consume .trlc sources,
# not the current .rst prose), and there are no system-level integration
# tests beyond the unit tests already wired via unit().
dependable_element(
    name = "dependable_element_bitmanipulation",
    assumptions_of_use = [":aous_bitmanipulation"],
    requirements = ["@score_platform//docs/features/baselibs/requirements:feat_req_baselibs"],
    architectural_design = [":arch_design_bitmanipulation"],
    dependability_analysis = [],
    components = [":component_bitmanipulation"],
    tests = [],
    integrity_level = "B",
    visibility = ["//visibility:public"],
)

docs_bundle(
    name = "docs",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)
