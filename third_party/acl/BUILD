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

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@rules_cc//cc:defs.bzl", "cc_library")

# Bazel-native equivalent of upstream acl's `./configure --enable-nls`: gates
# whether include/misc.h's `_()` macro (see @acl-src//:acl) routes through gettext().
bool_flag(
    name = "enable_nls",
    build_setting_default = False,
)

config_setting(
    name = "nls_enabled",
    flag_values = {":enable_nls": "True"},
    visibility = ["@acl-src//:__pkg__"],
)

# Hand-written stand-in for acl's autoconf-generated include/config.h, consumed by
# @acl-src (see MODULE.bazel and acl.BUILD) which vendors libacl from source.
cc_library(
    name = "config_h",
    hdrs = ["config.h"],
    defines = select({
        ":nls_enabled": ["ENABLE_NLS=1"],
        "//conditions:default": [],
    }),
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
    actual = "@acl-src//:acl",
    visibility = ["@score_baselibs//score/os:__pkg__"],
)

alias(
    name = "acl_shared",
    actual = "@acl-src//:acl_shared",
    visibility = ["//:__subpackages__"],
)
