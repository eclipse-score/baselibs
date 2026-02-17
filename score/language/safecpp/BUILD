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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    test_suites_from_sub_packages = [
        "@score_baselibs//score/language/safecpp/aborts_upon_exception:unit_tests",
        "@score_baselibs//score/language/safecpp/safe_math:unit_tests",
        "@score_baselibs//score/language/safecpp/scoped_function:unit_tests",
        "@score_baselibs//score/language/safecpp/string_view:unit_tests",
        "@score_baselibs//score/language/safecpp/test:unit_tests",
    ],
    visibility = [
        "//visibility:public",
    ],
)
