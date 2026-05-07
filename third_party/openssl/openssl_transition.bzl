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
warnings under strict C11 compilation. This transition:
  - Switches the C standard to gnu11 (required for OpenSSL's GNU extensions)
  - Suppresses warnings that OpenSSL legitimately triggers but are irrelevant
    to our codebase's quality standards

The transition is applied only to the non-QNX case (see BUILD), since QNX
uses a separately maintained OpenSSL target.
"""

_OPENSSL_EXTRA_COPTS = [
    "-std=gnu11",
    "-Wno-discarded-qualifiers",
    "-Wno-cast-qual",
    "-Wno-format",
    "-Wno-format-nonliteral",
    "-Wno-redundant-decls",
    "-Wno-suggest-attribute=format",
    "-Wno-bad-function-cast",
    "-Wno-implicit-function-declaration",
]

def _openssl_transition_impl(settings, attr):
    return {
        "//command_line_option:copt": list(settings["//command_line_option:copt"]) + _OPENSSL_EXTRA_COPTS,
    }

_openssl_transition = transition(
    implementation = _openssl_transition_impl,
    inputs = ["//command_line_option:copt"],
    outputs = ["//command_line_option:copt"],
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
