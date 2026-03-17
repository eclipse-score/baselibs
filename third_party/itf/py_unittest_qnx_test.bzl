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

def py_unittest_qnx_test(
        name,
        test_cases = [],
        test_suites = [],
        visibility = None,
        tags = [],
        excluded_tests_filter = None,
        cc_test_qnx = None,
        **kwargs):
    """Creates a test suite of QNX tests from cc_test targets.

    Args:
        name: Name of the test_suite to create
        test_cases: List of cc_test targets to wrap with cc_test_qnx
        test_suites: Additional test_suite targets to include
        visibility: Visibility for the generated test_suite
        tags: Tags to apply to the test_suite
        cc_test_qnx: Optional macro to wrap each cc_test target for QNX execution.
            The calling project provides its own cc_test_qnx implementation.
            If None, test_cases are ignored and only test_suites are collected.
        **kwargs: Additional arguments passed to test_suite
    """
    qnx_test_targets = []

    for test_case in test_cases:
        if cc_test_qnx != None:
            test_name = test_case.split(":")[-1] if ":" in test_case else test_case
            qnx_target_name = test_name + "_qnx"

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
