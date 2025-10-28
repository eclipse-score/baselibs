load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

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

package(
    default_visibility = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:__subpackages__",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":resource_manager_test",
        ":resource_test",
        ":static_c_wrappers_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":error_test",
        ":resource_manager_test",
        ":resource_test",
        ":static_c_wrappers_test",
        ":simple_read_function_test",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "resource_manager",
    srcs = [
        "src/resource_manager.cpp",
    ],
    hdrs = [
        "include/resource_manager.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "FUSA",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/utils/qnx/resource_manager:resource",
        "@score_baselibs//score/mw/log",
    ],
)

cc_library(
    name = "resource",
    srcs = [
        "src/resource.cpp",
        "src/static_c_wrappers.cpp",
    ],
    hdrs = [
        "include/resource.h",
        "include/static_c_wrappers.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "FUSA",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/os/qnx:types",
    ],
)

cc_library(
    name = "simple_read_function",
    srcs = [
        "src/simple_read_function.cpp",
    ],
    hdrs = [
        "include/simple_read_function.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "FUSA",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":error",
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/qnx:resmgr",
        "@score_baselibs//score/os/qnx:types",
        "@score_baselibs//score/mw/log",
    ],
)

cc_library(
    name = "handlers_mock",
    testonly = True,
    srcs = [
        "tests/mocks/open_function_mock.cpp",
        "tests/mocks/read_function_mock.cpp",
        "tests/mocks/seek_function_mock.cpp",
        "tests/mocks/write_function_mock.cpp",
    ],
    hdrs = [
        "tests/mocks/open_function_mock.h",
        "tests/mocks/read_function_mock.h",
        "tests/mocks/seek_function_mock.h",
        "tests/mocks/write_function_mock.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/utils/qnx/resource_manager:resource",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "handlers_stub",
    testonly = True,
    srcs = [
        "tests/stubs/open_function_stub.cpp",
        "tests/stubs/read_function_stub.cpp",
        "tests/stubs/seek_function_stub.cpp",
        "tests/stubs/write_function_stub.cpp",
    ],
    hdrs = [
        "tests/stubs/open_function_stub.h",
        "tests/stubs/read_function_stub.h",
        "tests/stubs/seek_function_stub.h",
        "tests/stubs/write_function_stub.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/utils/qnx/resource_manager:resource",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "resource_test",
    size = "small",
    srcs = [
        "tests/resource_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # Remove when CI will start supporting 'os:qnx' test target.
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:handlers_stub",
        "@score_baselibs//score/os/utils/qnx/resource_manager:resource",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "resource_manager_test",
    size = "small",
    srcs = [
        "tests/resource_manager_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # Remove when CI will start supporting 'os:qnx' test target.
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score_baselibs//score/os/mocklib/qnx:dispatch_mock",
        "@score_baselibs//score/os/mocklib/qnx:iofunc_mock",
        "@score_baselibs//score/os/utils/qnx/resource_manager",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "static_c_wrappers_test",
    size = "small",
    srcs = [
        "tests/static_c_wrappers_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # Remove when CI will start supporting 'os:qnx' test target.
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:handlers_mock",
        "@score_baselibs//score/os/utils/qnx/resource_manager:resource",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "simple_read_function_mock",
    testonly = True,
    srcs = [
        "tests/mocks/mock_simple_read_function.cpp",
    ],
    hdrs = [
        "tests/mocks/mock_simple_read_function.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:simple_read_function",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "simple_read_function_stub",
    srcs = [
        "tests/stubs/simple_read_function_stub.cpp",
    ],
    hdrs = [
        "tests/stubs/simple_read_function_stub.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "//platform/aas/unit/qnx/resource_manager:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/qnx:resmgr",
        "@score_baselibs//score/os/utils/qnx/resource_manager:simple_read_function",
    ],
)

cc_library(
    name = "simple_read_function_test_fixture",
    srcs = [
        "tests/simple_read_function_test_fixture.cpp",
    ],
    hdrs = [
        "tests/simple_read_function_test_fixture.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:__subpackages__",
    ],
    deps = [
        "@score_baselibs//score/os/qnx:channel",
        "@score_baselibs//score/os/qnx:dispatch",
        "@score_baselibs//score/os/qnx:iofunc",
        "@score_baselibs//score/os/qnx:resmgr",
        "@score_baselibs//score/os/utils/qnx/resource_manager:simple_read_function_stub",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "simple_read_function_test",
    srcs = ["tests/simple_read_function_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    target_compatible_with = ["@platforms//os:qnx"],
    deps = [
        "@score_baselibs//score/os/mocklib/qnx:iofunc_mock",
        "@score_baselibs//score/os/mocklib/qnx:resmgr_mock",
        "@score_baselibs//score/os/utils/qnx/resource_manager:simple_read_function_mock",
        "@score_baselibs//score/os/utils/qnx/resource_manager:simple_read_function_test_fixture",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "error",
    srcs = [
        "src/error.cpp",
    ],
    hdrs = [
        "include/error.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "FUSA",
    ],
    visibility = [
        "@score_baselibs//score/os/utils/qnx/resource_manager:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/result",
    ],
)

cc_test(
    name = "error_test",
    srcs = ["tests/error_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    deps = [
        ":error",
        "@googletest//:gtest_main",
    ],
)
