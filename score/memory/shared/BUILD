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

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

# --------------------------------------------------------------------------------------------------------------
# Migration shims for AtomicIndirector, which moved to //score/concurrency.
#
# The headers below only forward-include the new headers and pull the relocated symbols back into
# score::memory::shared via using-declarations, so existing bazel targets, include paths and namespace usages of
# consumers outside of this package keep working unchanged. New code should depend on
# @score_baselibs//score/concurrency directly.
# --------------------------------------------------------------------------------------------------------------

cc_library(
    name = "atomic_interface",
    srcs = ["i_atomic.cpp"],
    hdrs = ["i_atomic.h"],
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/concurrency:atomic_interface instead."),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/memory/shared:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/concurrency:atomic_interface",
    ],
)

cc_library(
    name = "atomic_indirector",
    srcs = ["atomic_indirector.cpp"],
    hdrs = ["atomic_indirector.h"],
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/concurrency:atomic_indirector instead."),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/concurrency:atomic_indirector",
    ],
)

cc_library(
    name = "atomic_indirector_mock_binding",
    testonly = True,
    srcs = ["atomic_mock.cpp"],
    hdrs = ["atomic_mock.h"],
    deprecation = ("This target is deprecated. Use " +
                   "@score_baselibs//score/concurrency:atomic_indirector_mock_binding instead."),
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/concurrency:atomic_indirector_mock_binding",
    ],
)
