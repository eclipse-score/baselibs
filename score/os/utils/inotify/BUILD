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
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "inotify_event",
    srcs = ["inotify_event.cpp"],
    hdrs = ["inotify_event.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":inotify_watch_descriptor",
        "@score-baselibs//score/os:inotify",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "inotify_instance",
    srcs = ["inotify_instance.cpp"],
    hdrs = ["inotify_instance.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":inotify_event",
        ":inotify_watch_descriptor",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "inotify_instance_impl",
    srcs = ["inotify_instance_impl.cpp"],
    hdrs = ["inotify_instance_impl.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":inotify_event",
        ":inotify_instance",
        ":inotify_watch_descriptor",
        "@score-baselibs//score/os:errno",
        "@score-baselibs//score/os:fcntl",
        "@score-baselibs//score/os:inotify",
        "@score-baselibs//score/os:sys_poll",
        "@score-baselibs//score/os:unistd",
        "@score-baselibs//score/os/utils:abortable_blocking_reader",
        "@score-baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "inotify_instance_mock",
    testonly = True,
    srcs = ["inotify_instance_mock.cpp"],
    hdrs = ["inotify_instance_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":inotify_instance",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "inotify_instance_facade",
    testonly = True,
    srcs = ["inotify_instance_facade.cpp"],
    hdrs = ["inotify_instance_facade.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        ":inotify_instance",
        ":inotify_instance_mock",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "inotify_watch_descriptor",
    srcs = ["inotify_watch_descriptor.cpp"],
    hdrs = ["inotify_watch_descriptor.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//visibility:public", # platform_only
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "inotify_event_test.cpp",
        "inotify_instance_impl_test.cpp",
        "inotify_watch_descriptor_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = [
        "unit",
    ],
    visibility = [
        "@score-baselibs//score/os/utils/test:__pkg__",
    ],
    deps = [
        ":inotify_instance_impl",
        "@score-baselibs//score/os/mocklib:fcntl_mock",
        "@score-baselibs//score/os/mocklib:inotify_mock",
        "@score-baselibs//score/os/mocklib:sys_poll_mock",
        "@score-baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)
