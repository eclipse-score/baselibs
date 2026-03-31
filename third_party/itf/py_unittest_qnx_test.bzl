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
load("@score_qnx_unit_tests//:cc_test_qnx.bzl", "cc_test_qnx")

def py_unittest_qnx_test(name, test_cases = [], test_suites = [], **kwargs):
    all_tests = []

    for idx, test_case in enumerate(test_cases):
        cc_test_qnx(
            name = "%s_%s_%d" % (name, native.package_relative_label(test_case).name, idx),
            cc_test = test_case,
        )
        all_tests.append(":%s_%s_%d" % (name, native.package_relative_label(test_case).name, idx))

    native.test_suite(
        name = name,
        tests = all_tests + test_suites,
    )
