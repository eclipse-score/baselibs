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

load("@score_baselibs//:bazel/unit_tests.bzl", "cc_gtest_unit_test", "cc_unit_test_suites_for_host_and_qnx")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("@score_tooling//bazel/rules/score_module:score_module.bzl", "safety_element_out_of_context")

load("@rules_python//sphinxdocs:sphinx_docs_library.bzl", "sphinx_docs_library")


cc_library(
    name = "clock",
    testonly = True,
    srcs = ["clock.cpp"],
    hdrs = ["clock.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

cc_gtest_unit_test(
    name = "clock_tests",
    srcs = ["clock_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":clock",
        "@score_baselibs//score/concurrency/future",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "destruction_guard",
    srcs = ["destruction_guard.cpp"],
    hdrs = ["destruction_guard.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
)

cc_library(
    name = "condition_variable",
    srcs = ["condition_variable.cpp"],
    hdrs = ["condition_variable.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":destruction_guard",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "interruptible_conditional_variable_mock",
    testonly = True,
    srcs = ["interruptible_conditional_variable_mock.cpp"],
    hdrs = ["interruptible_conditional_variable_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":clock",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "condition_variable_tests",
    srcs = ["condition_variable_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":clock",
        ":condition_variable",
    ],
)

cc_library(
    name = "interruptible_wait",
    srcs = ["interruptible_wait.cpp"],
    hdrs = ["interruptible_wait.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":condition_variable",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "interruptible_wait_tests",
    srcs = ["interruptible_wait_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":interruptible_wait",
    ],
)

cc_library(
    name = "notification",
    srcs = ["notification.cpp"],
    hdrs = ["notification.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":destruction_guard",
        ":interruptible_wait",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "notification_tests",
    srcs = ["notification_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":notification",
    ],
)

cc_library(
    name = "interruptible_interprocess_condition_variable",
    srcs = ["interruptible_interprocess_condition_variable.cpp"],
    hdrs = ["interruptible_interprocess_condition_variable.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":condition_variable",
        "@score_baselibs//score/os/utils/interprocess:interprocess_conditional_variable",
        "@score_baselibs//score/os/utils/interprocess:interprocess_mutex",
    ],
)

cc_library(
    name = "executor",
    srcs = [
        "executor.cpp",
        "simple_task.cpp",
        "task.cpp",
    ],
    hdrs = [
        "executor.h",
        "simple_task.h",
        "task.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":task_result",
        "@score_baselibs//score/concurrency/future",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "task_result_base",
    srcs = [
        "task_result_base.cpp",
    ],
    hdrs = ["task_result_base.h"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        "@score_baselibs//score/concurrency/future",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "task_result",
    srcs = ["task_result.cpp"],
    hdrs = ["task_result.h"],
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":shared_task_result",
        ":task_result_base",
        "@score_baselibs//score/concurrency/future",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "shared_task_result",
    srcs = ["shared_task_result.cpp"],
    hdrs = ["shared_task_result.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":task_result_base",
        "@score_baselibs//score/concurrency/future",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "executor_tests",
    srcs = [
        "executor_test.cpp",
        "simple_task_test.cpp",
        "task_result_test.cpp",
        "task_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":executor",
        ":executor_mock",
        ":shared_task_result",
        ":task_result",
    ],
)

cc_library(
    name = "executor_mock",
    testonly = True,
    srcs = ["executor_mock.cpp"],
    hdrs = ["executor_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":executor",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "thread_pool",
    srcs = ["thread_pool.cpp"],
    hdrs = ["thread_pool.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":condition_variable",
        ":executor",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:pthread",
    ],
)

cc_gtest_unit_test(
    name = "thread_pool_tests",
    srcs = ["thread_pool_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":thread_pool",
        "@score_baselibs//score/concurrency:notification",
    ],
)

cc_library(
    name = "long_running_threads_container",
    srcs = ["long_running_threads_container.cpp"],
    hdrs = ["long_running_threads_container.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":condition_variable",
        ":executor",
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/os:pthread",
    ],
)

cc_gtest_unit_test(
    name = "long_running_threads_container_tests",
    srcs = ["long_running_threads_container_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":long_running_threads_container",
    ],
)

cc_library(
    name = "periodic_task",
    srcs = ["periodic_task.cpp"],
    hdrs = ["periodic_task.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":executor",
        ":interruptible_wait",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "periodic_task_tests",
    srcs = ["periodic_task_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":clock",
        ":executor_mock",
        ":interruptible_conditional_variable_mock",
        ":periodic_task",
    ],
)

cc_library(
    name = "delayed_task",
    srcs = ["delayed_task.cpp"],
    hdrs = ["delayed_task.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":executor",
        ":interruptible_wait",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_gtest_unit_test(
    name = "delayed_task_tests",
    srcs = ["delayed_task_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    deps = [
        ":clock",
        ":delayed_task",
        ":executor_mock",
        ":interruptible_conditional_variable_mock",
    ],
)

cc_library(
    name = "synchronized_queue",
    srcs = ["synchronized_queue.cpp"],
    hdrs = ["synchronized_queue.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",  # platform_only
    ],
    deps = [
        ":notification",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "valgrind_on_host",
    deps = select({
        "@platforms//os:qnx": [],
        "//conditions:default": ["//third_party/valgrind"],
    }),
)

cc_gtest_unit_test(
    name = "synchronized_queue_test",
    srcs = ["synchronized_queue_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    flaky = True,  # TODO: Remove flakiness (Ticket-178835)
    deps = [
        ":synchronized_queue",
        ":valgrind_on_host",
    ],
)

cc_library(
    name = "concurrency",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":condition_variable",
        ":delayed_task",
        ":executor",
        ":interruptible_interprocess_condition_variable",
        ":interruptible_wait",
        ":long_running_threads_container",
        ":notification",
        ":periodic_task",
        ":synchronized_queue",
        ":thread_pool",
        "@score_baselibs//score/concurrency/timed_executor",
        "@score_baselibs//score/concurrency/timed_executor:concurrent_timed_executor",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_test_suite",
    cc_unit_tests = [
        ":clock_tests",
        ":condition_variable_tests",
        ":delayed_task_tests",
        ":executor_tests",
        ":interruptible_wait_tests",
        ":notification_tests",
        ":periodic_task_tests",
        ":thread_pool_tests",
        ":long_running_threads_container_tests",
        ":synchronized_queue_test",
    ],
    test_suites_from_sub_packages = [
        "@score_baselibs//score/concurrency/future:unit_test_suite",
        "@score_baselibs//score/concurrency/timed_executor:unit_test_suite",
        "@score_baselibs//score/concurrency/thread_load_tracking:unit_test_suite",
    ],
    visibility = [
        "//visibility:public",
    ],
)


# ############################################################
# SAFETY ELEMENT OUT OF CONTEXT
# ############################################################
sphinx_docs_library(
    name = "assumptions_of_use",
    srcs = ["design/assumptions_of_use.md"],
)

sphinx_docs_library(
    name = "component_requirements",
    srcs = ["design/component_requirements.md"],
)

sphinx_docs_library(
    name = "architectural_design",
    srcs = ["design/architectural_design.md"],
)

sphinx_docs_library(
    name = "safety_analysis",
    srcs = ["design/safety_analysis.md"],
)

safety_element_out_of_context(
    name = "seooc_concurrency",
    architectural_design = ":architectural_design",
    assumptions_of_use = ":assumptions_of_use",
    component_requirements = ":component_requirements",
    implementations = [
        "//score/concurrency:clock",
        "//score/concurrency:concurrency",
        "//score/concurrency:condition_variable",
        "//score/concurrency:delayed_task",
        "//score/concurrency:destruction_guard",
        "//score/concurrency:executor",
        "//score/concurrency:executor_mock",
        "//score/concurrency/future:future",
        "//score/concurrency/future:test_types",
        "//score/concurrency:interruptible_conditional_variable_mock",
        "//score/concurrency:interruptible_interprocess_condition_variable",
        "//score/concurrency:interruptible_wait",
        "//score/concurrency:long_running_threads_container",
        "//score/concurrency:notification",
        "//score/concurrency:periodic_task",
        "//score/concurrency:shared_task_result",
        "//score/concurrency:synchronized_queue",
        "//score/concurrency:task_result",
        "//score/concurrency:task_result_base",
        "//score/concurrency/thread_load_tracking:thread_load_tracking",
        "//score/concurrency:thread_pool",
        "//score/concurrency/timed_executor:concurrent_timed_executor",
        "//score/concurrency/timed_executor:delayed_task",
        "//score/concurrency/timed_executor:periodic_task",
        "//score/concurrency/timed_executor:timed_executor",
        "//score/concurrency/timed_executor:timed_executor_mock",
        "//score/concurrency/timed_executor:timed_task",
    ],
    safety_analysis = ":safety_analysis",
    tests = [
        "//score/concurrency/future:unit_test_suite_host",
        "//score/concurrency/thread_load_tracking:unit_test_suite_host",
        "//score/concurrency/timed_executor:unit_test_suite_host",
        "//score/concurrency:unit_test_suite_host",
    ],
    visibility = ["//visibility:public"],
)
