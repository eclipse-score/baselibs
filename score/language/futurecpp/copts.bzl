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

"""futurecpp-specific options for the internal targets.

Compiler and linker options shall not leak to client code. Therefore, only futurecpp-internal targets may use them.
"""

_SCORE_LANGUAGE_FUTURECPP_GCC_TEST_COPTS = [
    "-pthread",
    "-Wall",
    "-Wcast-qual",
    "-Wconversion",
    "-Wconversion-null",
    "-Wextra",
    "-Wfloat-equal",
    "-Wmissing-declarations",
    "-Wno-error=deprecated-declarations",
    "-Woverlength-strings",
    "-Wpedantic",
    "-Wpointer-arith",
    "-Wshadow",
    "-Wsign-conversion",
    "-Wunused-local-typedefs",
    "-Wunused-result",
    "-Wvarargs",
    "-Wvla",
    "-Wwrite-strings",
]

_SCORE_LANGUAGE_FUTURECPP_QCC_TEST_COPTS = [
]

SCORE_LANGUAGE_FUTURECPP_TEST_COPTS = select({
    "@platforms//os:linux": _SCORE_LANGUAGE_FUTURECPP_GCC_TEST_COPTS,
    "@platforms//os:macos": _SCORE_LANGUAGE_FUTURECPP_GCC_TEST_COPTS,
    "@platforms//os:qnx": _SCORE_LANGUAGE_FUTURECPP_QCC_TEST_COPTS,
})

_SCORE_LANGUAGE_FUTURECPP_GCC_TEST_LINKOPTS = [
    "-pthread",
]

_SCORE_LANGUAGE_FUTURECPP_QCC_TEST_LINKOPTS = [
]

SCORE_LANGUAGE_FUTURECPP_TEST_LINKOPTS = select({
    "@platforms//os:linux": _SCORE_LANGUAGE_FUTURECPP_GCC_TEST_LINKOPTS,
    "@platforms//os:macos": _SCORE_LANGUAGE_FUTURECPP_GCC_TEST_LINKOPTS,
    "@platforms//os:qnx": _SCORE_LANGUAGE_FUTURECPP_QCC_TEST_LINKOPTS,
})

SCORE_LANGUAGE_FUTURECPP_ASSERT_DEFINES = ["SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_DEBUG"]
