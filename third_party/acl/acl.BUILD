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

# BUILD file for the vendored acl-2.4.0 source (https://savannah.nongnu.org/projects/acl),
# used as the `build` file for the `acl-src` download_archive repository declared in
# MODULE.bazel. Only the plain POSIX ACL API is vendored (see srcs/hdrs below).
#
# acl_get_file(), acl_set_file(), acl_extended_file() and acl_extended_file_nofollow()
# are thin AT_FDCWD wrappers around the fd-relative acl_get_file_at()/acl_set_file_at()/
# acl_extended_file_at(), so those three "_at" files (and the libmisc getxattrat/
# setxattrat compat shims they need, since this toolchain's glibc predates those
# syscalls) are vendored too. acl_delete_def_file_at.c and perm_copy_fd.c/
# perm_copy_file.c (which pulls in a separate libattr dependency via
# <attr/error_context.h>) are intentionally excluded because score/os/acl_impl.cpp
# never calls them.

load("@rules_cc//cc:cc_shared_library.bzl", "cc_shared_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_python//python:defs.bzl", "py_test")
load("@score_baselibs//third_party/acl:acl_sources.bzl", "ACL_HDRS", "ACL_SRCS")

# include/acl.h and include/libacl.h declare their public API with a bare `EXPORT`
# marker. Upstream's install rule (include/Makemodule.am's SUBST_INSTALL_HEADER)
# rewrites `EXPORT` to `extern` via sed when installing these headers; since we vendor
# the headers directly instead of installing them, replicate that substitution with a
# transitive `defines` so any consumer compiling against these headers gets it too.

# include/acl.h remapped to the angle-include path <sys/acl.h>, matching score/os/acl.h.
cc_library(
    name = "sys_acl_h",
    hdrs = ["include/acl.h"],
    defines = ["EXPORT=extern"],
    include_prefix = "sys",
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

# include/libacl.h remapped to the angle-include path <acl/libacl.h>, matching score/os/acl.h.
cc_library(
    name = "acl_libacl_h",
    hdrs = ["include/libacl.h"],
    defines = ["EXPORT=extern"],
    include_prefix = "acl",
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

# Static compile-time interface, kept private to the one wrapper that needs it
# (score/os:acl); everyone else must go through :acl_shared instead.
cc_library(
    name = "acl",
    srcs = ACL_SRCS,
    hdrs = ACL_HDRS,
    includes = [
        "include",
        # Several vendored files quote-include with a redundant leading directory
        # (e.g. "include/visibility-hidden.h", "libmisc/proc-self-fd.h"); adding the
        # repo root lets those resolve as-is instead of needing per-file patches.
        ".",
    ],
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["@score_baselibs//third_party/acl:__pkg__"],
    deps = [
        ":acl_libacl_h",
        ":sys_acl_h",
        "@score_baselibs//third_party/acl:config_h",
    ],
)

# libacl.so, for consumers that want to link against acl dynamically instead of
# statically (e.g. so the vendored LGPL code can be replaced at runtime without
# relinking, per LGPL-2.1 section 6).
cc_shared_library(
    name = "acl_shared",
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:public"],
    deps = [":acl"],
)

# Fails if a vendored source/header references an autoconf-style feature macro
# (HAVE_*, ENABLE_*, etc.) that config.h doesn't define or document as intentionally
# unset, or if a vendored file's header claims the GPL rather than the LGPL, so both
# kinds of drift are caught automatically on every acl version bump or file-list change.
py_test(
    name = "config_drift_test",
    srcs = ["@score_baselibs//third_party/acl:check_config_drift.py"],
    main = "@score_baselibs//third_party/acl:check_config_drift.py",
    args = ["$(location @score_baselibs//third_party/acl:config.h)"] +
           ["$(location %s)" % f for f in ACL_SRCS + ACL_HDRS],
    data = ACL_SRCS + ACL_HDRS + ["@score_baselibs//third_party/acl:config.h"],
    visibility = ["//visibility:public"],
)
