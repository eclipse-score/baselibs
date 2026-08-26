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

# score_tooling's unit()/component() macros run a libclang-based C++ parser
# (see @score_tooling//cpp/libclang) but deliberately don't register a
# libclang toolchain themselves, so every consuming repository must register
# its own (see @score_tooling//cpp/libclang:libclang_toolchain.bzl docstring).
# We wrap the LLVM distribution already pulled in via the "llvm" module
# extension in //MODULE.bazel (@llvm_toolchain_llvm).
load("@score_tooling//cpp/libclang:libclang_toolchain.bzl", "libclang_toolchain")

libclang_toolchain(
    name = "baselibs_libclang",
    cxx_builtin_include = "@llvm_toolchain_llvm//:cxx_builtin_include",
    extra_config_site = "@llvm_toolchain_llvm//:extra_config_site",
    libclang = "@llvm_toolchain_llvm//:libclang",
    visibility = ["//visibility:public"],
)

toolchain(
    name = "baselibs_libclang_toolchain",
    toolchain = ":baselibs_libclang",
    toolchain_type = "@score_tooling//cpp/libclang:libclang_toolchain_type",
    visibility = ["//visibility:public"],
)
