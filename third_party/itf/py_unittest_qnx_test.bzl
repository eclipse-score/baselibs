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

"""Convenience macro for creating QNX test suites."""

load("//third_party/itf:cc_test_qnx.bzl", "cc_test_qnx")

def py_unittest_qnx_test(
        name,
        test_cases = [],
        test_suites = [],
        visibility = None,
        tags = [],
        excluded_tests_filter = None,
        **kwargs):
    """Creates a test suite of QNX tests from cc_test targets.

    Args:
        name: Name of the test_suite to create
        test_cases: List of cc_test targets to wrap with cc_test_qnx
        test_suites: Additional test_suite targets to include
        visibility: Visibility for the generated test_suite
        tags: Tags to apply to the test_suite
        **kwargs: Additional arguments passed to test_suite
    """
    qnx_test_targets = []

    for test_case in test_cases:
        # Generate a unique name from the full label to avoid conflicts
        # e.g. "@score_baselibs//score/os/utils/acl:unit_test" -> "score_baselibs__score_os_utils_acl__unit_test_qnx"
        unique_name = test_case.replace("@", "").replace("//", "__").replace("/", "_").replace(":", "__")
        qnx_target_name = unique_name + "_qnx"

        cc_test_qnx(
            name = qnx_target_name,
            cc_test = test_case,
        )

        qnx_test_targets.append(":" + qnx_target_name)

    native.test_suite(
        name = name,
        tests = qnx_test_targets + test_suites,
        visibility = visibility,
        tags = tags,
    )
