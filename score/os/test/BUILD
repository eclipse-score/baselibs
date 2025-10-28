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
load("@score_baselibs//third_party/itf:py_unittest_qnx_test.bzl", "py_unittest_qnx_test")

package(default_visibility = [
    "@score_baselibs//score/os/test:__subpackages__",
])

test_suite(
    name = "unit_tests_host",
    tests = [
        ":acl_test",
        ":arpa_inet_test",
        ":asil_qm_test",
        ":capability_test",
        ":cpuid_test",
        ":dirent_test",
        ":errno_test",
        ":fcntl_test",
        ":file_stream_test",
        ":ftw_test",
        ":glob_test",
        ":grp_test",
        ":ifaddrs_test",
        ":inotify_test",
        ":ioctl_test",
        ":memory_allocator_test",
        ":mount_test",
        ":net_if_test",
        ":netdb_test",
        ":objectseam_test",
        ":pthread_test",
        ":sched_mock_test",
        ":select_test",
        ":semaphore_test",
        ":sigevent_test",
        ":socket_test",
        ":stdio_test",
        ":stdlib_impl_test",
        ":string_impl_test",
        ":sys_wait_impl_test",
        ":uname_test",
        ":version_test",
        "@score_baselibs//score/os/test/linux:unit_tests_linux",
    ],
    visibility = [
        "//visibility:public",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":acl_test",
        ":arpa_inet_test",
        ":asil_qm_test",
        ":capability_test",
        ":cpuid_test",
        ":dirent_test",
        ":errno_test",
        ":fcntl_test",
        ":file_stream_test",
        ":ftw_test",
        ":getopt_test",
        ":glob_test",
        ":grp_test",
        ":ifaddrs_test",
        ":inotify_test",
        ":ioctl_test",
        ":libgen_test",
        ":makedev_test",
        ":memory_allocator_test",
        ":mount_test",
        ":mqueue_test",
        ":net_if_test",
        ":netdb_test",
        ":objectseam_test",
        ":pthread_test",
        ":sched_mock_test",
        ":sched_test",
        ":select_test",
        ":semaphore_test",
        ":sigevent_test",
        ":socket_test",
        ":spawn_test",
        ":stat_test",
        ":statvfs_test",
        ":stdio_test",
        ":stdlib_impl_test",
        ":string_impl_test",
        ":string_test",
        ":sys_poll_test",
        ":sys_uio_test",
        ":sys_wait_impl_test",
        ":time_test",
        ":uname_test",
        ":version_test",
    ],
    test_suites = [
        "@score_baselibs//score/os/test/qnx:unit_tests_qnx",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_test(
    name = "netdb_test",
    srcs = ["netdb_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:arpa_inet",
        "@score_baselibs//score/os:netdb",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "cpuid_test",
    srcs = ["cpuid_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:cpuid",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "net_if_test",
    srcs = ["net_if_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = [
        "unit",
    ],
    deps = [
        "@score_baselibs//score/os:net_if",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ifaddrs_test",
    srcs = ["ifaddrs_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:ifaddrs",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "file_stream_test",
    srcs = ["file_stream_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:file_stream",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "arpa_inet_test",
    srcs = ["arpa_inet_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:arpa_inet",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "memory_allocator_test",
    srcs = ["memory_allocator_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    target_compatible_with = select({
        "@platforms//os:qnx": ["//bazel/platforms/qnx_version:qnx7"],  # TODO: Ticket-214211 QNX8 incompatible
        "//conditions:default": [],
    }),
    deps = [
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sched_test",
    srcs = ["sched_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["manual"],
    deps = [
        "@score_baselibs//score/os:sched",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sys_poll_test",
    srcs = ["sys_poll_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["manual"],
    deps = [
        "@score_baselibs//score/os:sys_poll",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sys_uio_test",
    srcs = ["sys_uio_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:sys_uio",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "errno_test",
    srcs = ["errno_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os:errno_logging",
        "@score_baselibs//score/mw/log",
        "@score_baselibs//score/mw/log:recorder_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "mqueue_test",
    srcs = ["mqueue_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:mqueue",
        "@score_baselibs//score/os/utils/test/lib:random_string_test_lib",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "statvfs_test",
    srcs = ["statvfs_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:statvfs",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "libgen_test",
    srcs = ["libgen_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["manual"],
    deps = [
        "@score_baselibs//score/os:libgen",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "time_test",
    srcs = ["time_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["manual"],
    deps = [
        "@score_baselibs//score/os:time",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "fcntl_test",
    srcs = [
        "fcntl_impl_test.cpp",
        "fcntl_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/utils:scoped_operation",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "stat_test",
    srcs = ["stat_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        "@score_baselibs//score/os:stat",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "inotify_test",
    srcs = [
        "inotify_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os:inotify",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ioctl_test",
    srcs = [
        "ioctl_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = [
    ] + select({
        "@platforms//os:qnx": ["-lsocket"],
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:errno",
        "@score_baselibs//score/os/mocklib:ioctl_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "acl_test",
    srcs = [
        "acl_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:acl",
        "@score_baselibs//score/os:errno",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "spawn_test",
    srcs = [
        "spawn_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["manual"],
    target_compatible_with = select({
        "@platforms//os:qnx": ["//bazel/platforms/qnx_version:qnx7"],  # TODO: Ticket-214211 QNX8 incompatible
        "//conditions:default": [],
    }),
    deps = [
        "@score_baselibs//score/os:spawn",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "semaphore_test",
    srcs = [
        "semaphore_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:semaphore",
        "@score_baselibs//score/os/utils/test/lib:random_string_test_lib",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "pthread_test",
    srcs = ["pthread_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:pthread",
        "@score_baselibs//score/os/mocklib:pthread_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "uname_test",
    srcs = ["uname_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:uname",
        "@score_baselibs//score/os/mocklib:uname_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ftw_test",
    srcs = ["ftw_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = [
        "unit",
    ],
    deps = [
        "@score_baselibs//score/os/mocklib:ftw_mock",
        "@score_baselibs//score/os/posix:ftw_posix",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "objectseam_test",
    srcs = [
        "ObjectSeam_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:fcntl",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "socket_test",
    srcs = [
        "socket_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:unistd",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "asil_qm_test",
    srcs = [
        "mmanTest.cpp",
        "unistdTest.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:mman",
        "@score_baselibs//score/os:socket",
        "@score_baselibs//score/os:unistd",
        "@score_baselibs//score/os/utils:signal",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_test(
    name = "sched_mock_test",
    srcs = ["sched_mock_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os/mocklib:sched_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "capability_test",
    srcs = [
        "capability_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os/mocklib:capability_mock",
        "@score_baselibs//score/os/mocklib:unistd_mock",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "dirent_test",
    srcs = [
        "dirent_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:dirent",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "stdio_test",
    srcs = ["stdio_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:stdio",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "grp_test",
    srcs = ["grp_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:grp",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "stdlib_impl_test",
    srcs = ["stdlib_impl_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:stdlib",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "makedev_test",
    srcs = ["makedev_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:make_dev",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "string_impl_test",
    srcs = [
        "string_impl_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:string",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sys_wait_impl_test",
    srcs = [
        "sys_wait_impl_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:sys_wait",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "select_test",
    srcs = [
        "select_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:select",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "getopt_test",
    srcs = ["getopt_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:getopt",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "string_test",
    srcs = ["string_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:string",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "mount_test",
    srcs = ["mount_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:mount",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "glob_test",
    srcs = [
        "glob_impl_test.cpp",
        "glob_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@score_baselibs//score/os:glob",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "sigevent_test",
    srcs = ["sigevent_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    deps = [
        "@score_baselibs//score/os:sigevent",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "version_test",
    srcs = ["version_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    tags = [
        "manual",  # to disable using as host unit test in CI
        "unit",
    ],
    deps = [
        "@score_baselibs//score/os:version",
        "@googletest//:gtest_main",
    ],
)
