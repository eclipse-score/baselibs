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

"""User-defined transition for building OpenSSL with relaxed compiler settings.

OpenSSL is a C library that uses GNU extensions and patterns that trigger
warnings under strict compilation settings. This transition adjusts the
active toolchain features rather than injecting raw compiler flags, making
it portable across all integrated toolchains:

  - Enables  "gnu11"            so the toolchain applies the GNU C11 dialect
                                 required by OpenSSL's GNU extensions.
  - Disables "minimal_warnings", "strict_warnings", "warnings_as_errors"
                                 so OpenSSL's legitimate warning patterns do
                                 not cause build failures.

Each toolchain translates these feature names into its own concrete flags,
keeping this transition platform-agnostic.

The transition is applied only to the non-QNX case (see BUILD), since QNX
uses a separately maintained OpenSSL target.
"""

_FEATURES_TO_DISABLE = [
    "minimal_warnings",
    "strict_warnings",
    "warnings_as_errors",
]

def _openssl_transition_impl(settings, attr):
    current = list(settings["//command_line_option:features"])
    updated = []
    for f in current:
        if f not in _FEATURES_TO_DISABLE:
            updated.append(f)
    updated.append("gnu11")
    return {
        "//command_line_option:features": updated,
    }

_openssl_transition = transition(
    implementation = _openssl_transition_impl,
    inputs = ["//command_line_option:features"],
    outputs = ["//command_line_option:features"],
)

def _openssl_library_impl(ctx):
    # When a Starlark transition is applied to attr.label, Bazel wraps the
    # result in a list (one entry per output configuration of the transition).
    dep = ctx.attr.dep[0]
    return [dep[DefaultInfo], dep[CcInfo]]

openssl_library = rule(
    implementation = _openssl_library_impl,
    attrs = {
        "dep": attr.label(
            cfg = _openssl_transition,
            providers = [CcInfo],
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
)
