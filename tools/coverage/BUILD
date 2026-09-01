# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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

load("@score_tooling//coverage:defs.bzl", "score_coverage_reporter", "score_coverage_scope")

# Coverage scope: every production (non-testonly) cc_library / rust_library
# under //score/..., matching the surface the legacy `bazel coverage
# //score/...` gcov flow instrumented (eclipse-score/baselibs#512). Only the
# roots of the dependency forest are listed - the scope aspect walks
# transitive in-workspace deps. QNX-only libraries are excluded (they cannot
# be built on the Linux host; on-target coverage: eclipse-score/tooling#427).
#
# Regenerate the list after adding/removing libraries:
#   bazel query 'kind("cc_library|rust_library", //score/...)
#       except attr(testonly, 1, //score/...)
#       except attr("target_compatible_with", "qnx", //score/...)'
# then keep only targets no other target in the result depends on.
score_coverage_scope(
    name = "coverage_scope",
    testonly = True,
    visibility = ["//visibility:private"],
    deps = [
        # Public API alias of the generic trace library: reaches the
        # interface_types production libraries (the mock and the stub default
        # implementation are excluded - see scope_exclusions.txt).
        "//score/analysis/tracing/generic_trace_library:api",
        "//score/analysis/tracing/common/canary_wrapper",
        "//score/bitmanipulation",
        "//score/concurrency/thread_load_tracking",
        "//score/containers:dynamic_array",
        "//score/containers:intrusivelist",
        "//score/containers_rust:containers",
        "//score/datetime_converter:time_conversion",
        "//score/flatbuffers:flatbufferutils",
        "//score/hash",
        "//score/hash:safe_hash",
        "//score/hash/code/core:hash_cal_mock",
        "//score/hash/code/core/factory:factory_mock",
        "//score/hash/code/core/factory/impl:factory_headers",
        "//score/hash/code/crc:mocks",
        # Parser/writer-facing interface: not reachable from the impl libs
        # under the default base_library select; has a coverage grant.
        "//score/json:interface",
        "//score/json:json_serializer",
        "//score/json/internal/parser/vajson:vajson_parser",
        "//score/language/rust/memres",
        "//score/language/rust/stop_token",
        "//score/language/safecpp/aborts_upon_exception:abortsuponexception",
        "//score/language/safecpp/safe_atomics:try_atomic_add",
        "//score/language/safecpp/string_view:null_termination_check",
        "//score/log_rust/stdout_logger",
        "//score/log_rust/stdout_logger_cpp_init",
        "//score/memory:data_type_size_info",
        "//score/memory:endianness",
        "//score/memory:pmr_ring_buffer",
        "//score/mw/log:console",
        "//score/mw/log:minimal",
        "//score/mw/log:plugin_api",
        "//score/mw/log/configuration:nvconfigfactory",
        "//score/mw/log/detail:empty_recorder_factory",
        "//score/mw/log/detail:thread_local_guard",
        # NOT in scope: //score/mw/log/detail/utils/signal_handling - a
        # dependency-forest root whose restricted visibility cannot be
        # granted here: its BUILD file is under the restricted path
        # score/mw/** (internal-export process). Its files are absent
        # from the report until a grant is exported internally.
        "//score/network:netutils",
        "//score/network:udp_socket",
        "//score/network:vlan",
        "//score/network/sock_async:socket_factory",
        "//score/os:arpa_inet",
        "//score/os:capability",
        "//score/os:dlfcn",
        "//score/os:getopt",
        "//score/os:glob",
        "//score/os:mman",
        "//score/os:mount",
        "//score/os:net_if",
        "//score/os:netdb",
        # Public select() aliases: reach the linux-only pthread_default /
        # rt_default implementation libraries (private, alias-only access).
        "//score/os:pthread",
        "//score/os:rt",
        "//score/os:sched",
        # Wrappers whose only in-repo consumers are mocks/tests; still shipped
        # production code (coverage visibility grants).
        "//score/os/linux",
        "//score/os/linux/utils:network_interface",
        "//score/os:semaphore",
        "//score/os:sigevent",
        "//score/os:spawn",
        "//score/os:statvfs",
        "//score/os:string",
        "//score/os:sys_uio",
        "//score/os:sys_wait",
        "//score/os:time",
        "//score/os/posix:ftw_posix",
        "//score/os/utils:detect_os",
        "//score/os/utils:signal",
        "//score/os/utils:machine",
        "//score/os/utils:mqueue",
        "//score/os/utils:spinlock",
        "//score/os/utils:thread",
        "//score/os/utils/acl:mock",
        "//score/os/utils/fake:machine_fake",
        "//score/os/utils/inotify:inotify_instance_impl",
        "//score/os/utils/interprocess:interprocess_notification",
        "//score/result:result_for_rust",
        "//score/result/rust:result_cxx",
        "//score/result/rust:result_rs",
        "//score/scope_exit",
        "//score/static_reflection_with_serialization/serialization",
        "//score/static_reflection_with_serialization/visitor/examples/ostream:visitor_examples",
        "//score/string_manipulation/arguments",
        "//score/sync",
        "//score/thread",
        "//score/utils:base64",
    ],
)

score_coverage_reporter(
    name = "reporter_wrapper",
    testonly = True,
    coverage_scope = ":coverage_scope",
    llvm_cov = "@llvm_toolchain//:llvm-cov",
    llvm_cxxfilt = "@llvm_toolchain_llvm//:bin/llvm-cxxfilt",
    llvm_profdata = "@llvm_toolchain//:llvm-profdata",
    visibility = ["//visibility:public"],
)
