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
load("@score_baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

def cc_unit_test_suites_for_host_and_qnx(name, cc_unit_tests = None, visibility = None, test_suites_from_sub_packages = None, excluded_tests_filter = None):
    """
    This Bazel macro allows to add unit tests on qnx and host with a single macro.

    The limitations are that you cannot specify data dependencies and you cannot say that a test is qnx or linux only.
    In case you are hit by one of these limitations, you can still do it manually as before without the macro.

    Args:
        name: Name that will be used to create the host and qnx test suites. The resulting name will be the provided one concatenated with "_host" for the host and "_qnx" for QNX.
        cc_unit_tests: A list of cc_test targets that should be part of both, the host and QNX test suite.
        visibility: The visibility that should have the host and QNX test suites.
        test_suites_from_sub_packages: The test suites from the sub packages that you would like to collect in the newly created test suite.
        excluded_tests_filter: list of tests to be excluded from execution.
            Examples:
            FooTest.Test1 - do not run Test1 from test suite FooTest
            FooTest.* - do not run any test from test suite FooTest
            *FooTest.* - runs all non FooTest tests.

    Returns:
        Test suites for host and QNX
    """

    if cc_unit_tests == None and test_suites_from_sub_packages == None:
        fail("Define 'cc_unit_tests' or 'test_suites_from_sub_packages'")

    _host_test_suites_from_sub_packages = [test_suite + "_host" for test_suite in test_suites_from_sub_packages] if test_suites_from_sub_packages else []
    _host_tests = cc_unit_tests + _host_test_suites_from_sub_packages if cc_unit_tests else _host_test_suites_from_sub_packages

    native.test_suite(
        name = name + "_host",
        tests = _host_tests,
        visibility = visibility,
    )

    if cc_unit_tests:
        py_unittest_qnx_test(
            name = name + "_qnx_cases",
            testonly = True,
            test_cases = cc_unit_tests,
            excluded_tests_filter = excluded_tests_filter,
        )

    _qnx_test_suites_from_sub_packages = [test_suite + "_qnx" for test_suite in test_suites_from_sub_packages] if test_suites_from_sub_packages else []
    _qnx_test_suites = [":" + name + "_qnx_cases"] + _qnx_test_suites_from_sub_packages if cc_unit_tests else _qnx_test_suites_from_sub_packages

    py_unittest_qnx_test(
        name = name + "_qnx",
        tags = ["manual"],
        test_suites = _qnx_test_suites,
        visibility = visibility,
    )

def cc_unit_test(name, **kwargs):
    """
    Macro in order to declare a C++ unit test

    Args:
      name: Target name, to be forwarded to cc_test
      **kwargs: Additional parameters to be forwarded to cc_test. size and timeout cannot be provided and if tags is
        provided, it should not contain the tag unit. The unit tag is added automatically.

    Wrapper to create a cc_test that provides the unit tag and it forces the size and the timeout of the test
    This makes sure that all unit tests are small and short. If for whatever reason you need that, do not think about
    changing this macro to provide an override. Instead, use directly cc_test.

    There are two parts that are not provided intentionally:
    - toolchain features for compiler warnings still need to be provided. The reason why it is not provided is
    to prevent that from the outside it looks like for some target we provide and some not. We can only do that
    if we have this option for the other cc_library targets.
    - dependencies for gtest. The reason is that this is not called cc_gtest_unit_test but generic, that is why for the
    moment we keep it like that. If you want to have gtest dependencies consider using cc_gtest_unit_test.
    """
    kwargs.setdefault("tags", [])
    if "unit" in kwargs["tags"]:
        fail("'unit' tag already provided, please refrain from adding it manually.")

    kwargs["tags"].append("unit")
    native.cc_test(
        name = name,
        size = "small",
        timeout = "short",
        **kwargs
    )

def cc_gtest_unit_test(name, **kwargs):
    """
    Macro in order to declare a C++ unit test

    Args:
      name: Target name, to be forwarded to cc_unit_test and transitively to cc_test

      **kwargs: Additional parameters to be forwarded to cc_unit_test and transitively to cc_test. size and timeout
      cannot be provided and if tags is provided, it should not contain the tag unit.
      The following dependencies are already added to deps:
      "@score_baselibs//score/language/safecpp/coverage_termination_handler", and "@googletest//:gtest_main".
      The following toolchain features are already added to features: "aborts_upon_exception".

    Wrapper to create a cc_unit_test that provides the most common dependencies to be used in a unit test of an ASIL B
    application. If for whatever reason you need to have some other common dependency or you cannot follow the restrictions
    of cc_unit_test, do not think about changing this macro to provide an override. Instead, use directly cc_test.

    Like with cc_unit_test, toolchain features for compiler warnings still need to be provided. The reason why it is not
    provided is to prevent that from the outside it looks like for some target we provide and some not. We can only do that
    if we have this option for the other cc_library targets.
    """

    # Avoid mutating input arguments
    deps = [
        "@score_baselibs//score/language/safecpp/coverage_termination_handler",
        "@googletest//:gtest_main",
    ]
    deps += kwargs.pop("deps", [])

    features = [
        "aborts_upon_exception",
    ]
    features += kwargs.pop("features", [])

    cc_unit_test(
        name = name,
        features = features,
        deps = deps,
        **kwargs
    )
