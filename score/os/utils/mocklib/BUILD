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
cc_library(
    name = "mqueue",
    testonly = True,
    srcs = ["mqueuemock.cpp"],
    hdrs = ["mqueuemock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:mqueue_headers",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "semaphore",
    testonly = True,
    srcs = ["semaphoremock.cpp"],
    hdrs = ["semaphoremock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os/utils:semaphore_headers",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "signal_mock",
    testonly = True,
    srcs = ["signalmock.cpp"],
    hdrs = ["signalmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:signal_headers",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "thread",
    testonly = True,
    srcs = ["threadmock.cpp"],
    hdrs = ["threadmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:thread_headers",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "tcp_keep_alivemock",
    testonly = True,
    srcs = ["tcp_keep_alivemock.cpp"],
    hdrs = ["tcp_keep_alivemock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:tcp_keep_alive",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "path_mock",
    testonly = True,
    hdrs = ["pathmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:path_header",
        "@googletest//:gtest",
    ],
)
