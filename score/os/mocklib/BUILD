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
    name = "acl_mock",
    testonly = True,
    srcs = ["acl_mock.cpp"],
    hdrs = ["acl_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:acl",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "pthread_mock",
    testonly = True,
    srcs = ["mock_pthread.cpp"],
    hdrs = ["mock_pthread.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:pthread",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "ftw_mock",
    testonly = True,
    srcs = ["mock_ftw.cpp"],
    hdrs = ["mock_ftw.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/interface:ftw",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "dirent_mock",
    testonly = True,
    srcs = ["mock_dirent.cpp"],
    hdrs = ["mock_dirent.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:dirent",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "select_mock",
    testonly = True,
    srcs = ["mock_select.cpp"],
    hdrs = ["mock_select.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:select",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "nonposixwrapper_mock",
    testonly = True,
    srcs = ["nonposixwrappermock.cpp"],
    hdrs = ["nonposixwrappermock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:file_stream",
        "@score_baselibs//score/os/linux:linux_header",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "pthreadwrapper_mock",
    testonly = True,
    srcs = ["pthreadwrappermock.cpp"],
    hdrs = ["pthreadwrappermock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:pthread",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "cpuid_mock",
    testonly = True,
    srcs = ["cpuidmock.cpp"],
    hdrs = ["cpuidmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:cpuid",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "machine_mock",
    testonly = True,
    hdrs = ["machine_mock.h"],
    features = ["test_warnings"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/utils:machine",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "file_stream_mock",
    testonly = True,
    srcs = ["file_stream_mock.cpp"],
    hdrs = ["file_stream_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:file_stream",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "getopt_mock",
    testonly = True,
    srcs = ["getoptmock.cpp"],
    hdrs = ["getoptmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:getopt",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "libgen_mock",
    testonly = True,
    srcs = ["libgen_mock.cpp"],
    hdrs = ["libgen_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:file_stream",
        "@score_baselibs//score/os:libgen",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "inotify_mock",
    testonly = True,
    srcs = ["inotify_mock.cpp"],
    hdrs = ["inotify_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:inotify",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "ioctl_mock",
    testonly = True,
    srcs = ["ioctl_mock.cpp"],
    hdrs = ["ioctl_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:ioctl",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "mman_mock",
    testonly = True,
    srcs = ["mman_mock.cpp"],
    hdrs = ["mman_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:mman",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "stdlib_mock",
    testonly = True,
    srcs = ["stdlib_mock.cpp"],
    hdrs = ["stdlib_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:stdlib",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "statvfs_mock",
    testonly = True,
    srcs = ["statvfsmock.cpp"],
    hdrs = ["statvfsmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:statvfs",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "fcntl_mock",
    testonly = True,
    srcs = ["fcntl_mock.cpp"],
    hdrs = ["fcntl_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:fcntl",
        "@score_baselibs//score/os:mman",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "stat_mock",
    testonly = True,
    srcs = ["stat_mock.cpp"],
    hdrs = ["stat_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:mman",
        "@score_baselibs//score/os:stat",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "stdio_mock",
    testonly = True,
    hdrs = ["stdiomock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:stdio",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "mount_mock",
    testonly = True,
    srcs = ["mount_mock.cpp"],
    hdrs = ["mount_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:mount",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "grp_mock",
    testonly = True,
    hdrs = ["grpmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:grp",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "unistd_mock",
    testonly = True,
    hdrs = ["unistdmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:unistd",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "string_mock",
    testonly = True,
    srcs = ["string_mock.cpp"],
    hdrs = ["string_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:string",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "time_mock",
    testonly = True,
    srcs = ["time_mock.cpp"],
    hdrs = ["time_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:time",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "mqueue_mock",
    testonly = True,
    srcs = ["mqueuemock.cpp"],
    hdrs = ["mqueuemock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:mqueue",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "net_if_mock",
    testonly = True,
    srcs = ["net_if_mock.cpp"],
    hdrs = ["net_if_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:net_if",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sys_poll_mock",
    testonly = True,
    srcs = ["sys_poll_mock.cpp"],
    hdrs = ["sys_poll_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:sys_poll",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sys_uio_mock",
    testonly = True,
    srcs = ["sys_uio_mock.cpp"],
    hdrs = ["sys_uio_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:sys_uio",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "semaphore_mock",
    testonly = True,
    srcs = ["semaphoremock.cpp"],
    hdrs = ["semaphoremock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:semaphore",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "socket_mock",
    testonly = True,
    srcs = ["socketmock.cpp"],
    hdrs = ["socketmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:socket",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "netdb_mock",
    testonly = True,
    srcs = ["netdbmock.cpp"],
    hdrs = ["netdbmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:netdb",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "uname_mock",
    testonly = True,
    srcs = ["uname_mock.cpp"],
    hdrs = ["uname_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:uname",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "arpa_inet_mock",
    testonly = True,
    srcs = ["arpa_inet_mock.cpp"],
    hdrs = ["arpa_inet_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:arpa_inet",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "pcap_mock",
    testonly = True,
    srcs = ["pcap_mock.cpp"],
    hdrs = ["pcap_mock.h"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/qnx:pcap",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "sys_wait_mock",
    testonly = True,
    srcs = ["sys_wait_mock.cpp"],
    hdrs = ["sys_wait_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:sys_wait",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "sched_mock",
    testonly = True,
    srcs = ["sched_mock.cpp"],
    hdrs = ["sched_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:sched",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "spawn_mock",
    testonly = True,
    srcs = ["spawn_mock.cpp"],
    hdrs = ["spawn_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:spawn",
        "@googletest//:gtest",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "ifaddrs_mock",
    testonly = True,
    srcs = ["ifaddrs_mock.cpp"],
    hdrs = ["ifaddrs_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/os:ifaddrs",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "capability_mock",
    testonly = True,
    srcs = ["capability_mock.cpp"],
    hdrs = ["capability_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//visibility:public", # platform_only
    ],
    deps = [
        "@score_baselibs//score/os:capability",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "linuxsched_mock",
    testonly = True,
    srcs = ["linuxschedmock.cpp"],
    hdrs = ["linuxschedmock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os/linux:linux_header",
        "@googletest//:gtest_main",
        "@score_baselibs//score/language/futurecpp",
    ],
)

cc_library(
    name = "glob_mock",
    testonly = True,
    srcs = ["glob_mock.cpp"],
    hdrs = ["glob_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:glob",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "sigevent_mock",
    testonly = True,
    srcs = ["sigevent_mock.cpp"],
    hdrs = ["sigevent_mock.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@score_baselibs//score/os:sigevent",
        "@googletest//:gtest",
    ],
)
