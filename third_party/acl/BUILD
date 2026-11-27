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

load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_import", "cc_library")

# `default` builds libacl from source; `system` links the libacl.so.1 of the target at run time.
string_flag(
    name = "acl_mode",
    build_setting_default = "default",
    values = [
        "default",
        "system",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "acl_mode_system",
    flag_values = {":acl_mode": "system"},
    visibility = ["//visibility:public"],
)

# Own declarations of the libacl API used by score/os; no libacl code or headers are vendored.
cc_library(
    name = "acl_system_headers",
    hdrs = [
        "system/include/acl/libacl.h",
        "system/include/sys/acl.h",
    ],
    includes = ["system/include"],
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:private"],
)

# Link-time stand-in with the soname libacl.so.1; the libacl.so.1 of the target is loaded at run time.
cc_binary(
    name = "libacl_link_stub.so",
    srcs = ["system/acl_link_stub.c"],
    linkopts = ["-Wl,-soname,libacl.so.1"],
    linkshared = True,
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:private"],
    deps = [":acl_system_headers"],
)

cc_import(
    name = "acl_link_stub",
    shared_library = ":libacl_link_stub.so",
    target_compatible_with = ["@platforms//os:linux"],
)

cc_library(
    name = "acl_system",
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:private"],
    deps = [
        ":acl_link_stub",
        ":acl_system_headers",
    ],
)

# Hand-written stand-in for acl's autoconf-generated include/config.h, consumed by
# @acl-src (see MODULE.bazel and acl.BUILD) which vendors libacl from source.
cc_library(
    name = "config_h",
    hdrs = ["config.h"],
    includes = ["."],
    visibility = ["@acl-src//:__pkg__"],
)

# Exposed as plain files (rather than only via the cc_library above) so
# @acl-src//:config_drift_test (see acl.BUILD) can read config.h as test data and
# run check_config_drift.py without needing a C++ toolchain.
exports_files(
    [
        "config.h",
        "check_config_drift.py",
    ],
    visibility = ["@acl-src//:__pkg__"],
)

# Static linking is not exposed to general consumers; score/os:acl is the only
# target that needs the static compile-time interface, everyone else must use
# :acl_shared (paired with dynamic_deps) instead.
alias(
    name = "acl",
    actual = select({
        ":acl_mode_system": ":acl_system",
        "//conditions:default": "@acl-src//:acl",
    }),
    visibility = ["@score_baselibs//score/os:__pkg__"],
)

alias(
    name = "acl_shared",
    actual = "@acl-src//:acl_shared",
    visibility = ["//:__subpackages__"],
)
