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
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_library(
    name = "mqueue_headers",
    hdrs = ["mqueue.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/os/utils/mocklib:__subpackages__"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "mqueue",
    srcs = ["mqueue.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":mqueue_headers",
        "@score_baselibs//score/os:mqueue",
        "@score_baselibs//score/os:stat",
        "@score_baselibs//score/utils:string_hash",
        "@score_baselibs//score/utils:time_conversion",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "semaphore_headers",
    hdrs = ["semaphore.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["@score_baselibs//score/os/utils/mocklib:__subpackages__"],
    deps = [
        "@score_baselibs//score/utils:pimpl_ptr",
    ],
)

cc_library(
    name = "semaphore",
    srcs = ["semaphore.cpp"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        ":semaphore_headers",
        "@score_baselibs//score/os:semaphore",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "signal_headers",
    hdrs = [
        "signal.h",
        "signal_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/os/utils/mocklib:__subpackages__"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "signal",
    srcs = [
        "signal.cpp",
        "signal_impl.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/os:__subpackages__",
        "//platform/aas/mw/lifecycle:__subpackages__",
        "@score_baselibs//score/mw/log:__subpackages__",
        "//platform/aas/pas/crash_reporter:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
    ],
    deps = [
        ":signal_headers",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "thread_headers",
    hdrs = ["thread.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/os/utils/mocklib:__subpackages__"],
)

cc_library(
    name = "thread",
    srcs = [
        "thread.cpp",
    ] + select({
        "@platforms//os:linux": ["thread_linux.cpp"],
        "@platforms//os:qnx": ["thread_qnx.cpp"],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":thread_headers",
        "@score_baselibs//score/os:pthread",
        "@score_baselibs//score/mw/log",
    ],
)

cc_library(
    name = "high_resolution_steady_clock",
    hdrs = ["high_resolution_steady_clock.h"],
    deprecation = "Deprecated, please use //platform/aas/mw/time/HighPrecisionLocalSteadyClock instead",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/analysis/tracing/generic_trace_library/details/trace_job_allocator:__pkg__",
        "@score_baselibs//score/analysis/tracing/generic_trace_library/test/unit_test:__pkg__",
        "@score_baselibs//score/analysis/tracing/plugin/ipc_trace_plugin/ipc_trace/utils:__pkg__",
        "@score_baselibs//score/os/utils:__pkg__",
        "@score_baselibs//score/os/utils/test:__pkg__",
        "@score_baselibs//score/mw/log/detail/data_router/shared_memory:__pkg__",
        "@score_baselibs//score/mw/log/detail/file_logging:__pkg__",
        "//platform/aas/pas/logging:__pkg__",
        "//platform/aas/sysfunc/SafeCarTime/code/apps/TimeSyncValidationNonFFI/factory/impl:__pkg__",
        "//platform/aas/sysfunc/SafeCarTime/code/factory/impl:__pkg__",
        "//platform/aas/test/pas/perf_tests/tools/perf_apps:__pkg__",
    ],
)

cc_library(
    name = "spinlock",
    srcs = ["spinlock.cpp"],
    hdrs = ["spinlock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "path_header",
    hdrs = [
        "path.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "path",
    srcs = [
        "path.cpp",
        "path_impl.h",
    ] + select({
        "@platforms//os:qnx": ["path_qnx.cpp"],
        "//conditions:default": ["path_linux.cpp"],
    }),
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/abac/abac/core/DataLogistic:__subpackages__",
        "@score_baselibs//score/os:__subpackages__",
        "@score_baselibs//score/mw/log:__subpackages__",
        "//platform/aas/mw/storage:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
        "//platform/aas/sec/secured:__subpackages__",
    ],
    deps = [
        ":path_header",
        "@score_baselibs//score/os:libgen",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "machine_header",
    hdrs = [
        "machine.h",
        "machine_seam.h",
        "machine_seam_impl.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "machine",
    srcs = [
        "machine.cpp",
        "machine_seam.cpp",
        "machine_seam_impl.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    local_defines = select({
        "@score_baselibs//bazel/settings/qemu:on_qemu": ["MACHINE_QEMU"],
        "//conditions:default": [],
    }),
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":machine_header",
        "@score_baselibs//score/os:stdlib",
    ],
)

cc_library(
    name = "detect_os",
    srcs = ["detect_os.cpp"],
    hdrs = ["detect_os.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:uname",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "tcp_keep_alive",
    srcs = select({
        "@platforms//os:linux": ["tcp_keep_alive_linux.cpp"],
        "@platforms//os:qnx": ["tcp_keep_alive_qnx.cpp"],
    }),
    hdrs = ["tcp_keep_alive.h"],
    features = COMPILER_WARNING_FEATURES,
    target_compatible_with = select({
        "@platforms//os:qnx": ["//bazel/platforms/qnx_version:qnx7"],  # TODO: Ticket-214211 QNX8 incompatible
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/mw/log",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "abortable_blocking_reader",
    srcs = ["abortable_blocking_reader.cpp"],
    hdrs = ["abortable_blocking_reader.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["@score_baselibs//score/os/utils:__subpackages__"],
    deps = [
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:sys_poll",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/language/futurecpp",
    ],
)
