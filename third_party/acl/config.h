/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
/*
 * Hand-written stand-in for acl's autoconf-generated include/config.h.
 *
 * Upstream acl (savannah.nongnu.org/projects/acl) is built with autotools, which
 * probes the host for optional features via `./configure` and emits this file.
 * We vendor libacl's source directly into a plain cc_library instead of running
 * autotools, so this file hardcodes the feature set for our only supported
 * target class: glibc-based 64-bit Linux (Ubuntu, RedHat AutoSD, Elektrobit
 * Linux) on x86_64/aarch64. It only defines what the vendored translation units
 * (see //third_party/acl:acl.BUILD) actually need.
 */
#ifndef SCORE_BASELIBS_THIRD_PARTY_ACL_CONFIG_H
#define SCORE_BASELIBS_THIRD_PARTY_ACL_CONFIG_H

/* Enable GNU/glibc extensions, as AC_USE_SYSTEM_EXTENSIONS would on a real configure run. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

/* Visibility attribute for API symbols: irrelevant for a statically linked cc_library. */
#define EXPORT extern

/* Standard C89/glibc headers: always present on the Linux flavors this repo targets. */
#define STDC_HEADERS 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_WCHAR_H 1
#define HAVE_DLFCN_H 1

/* Package metadata (informational; not consumed by the vendored subset). */
#define PACKAGE "acl"
#define PACKAGE_NAME "acl"
#define PACKAGE_VERSION "2.4.0"
#define PACKAGE_STRING "acl 2.4.0"
#define PACKAGE_TARNAME "acl"
#define VERSION "2.4.0"

/*
 * ENABLE_NLS is deliberately not hardcoded here: it's controlled by the
 * --@score_baselibs//third_party/acl:enable_nls Bazel flag instead (see BUILD's
 * config_h target), so consumers get real bazel-style control over it rather
 * than needing to fork this file.
 *
 * Intentionally left undefined (matches upstream's "not detected"/disabled state):
 * - HAVE_GETTEXT / HAVE_DCGETTEXT / HAVE_ICONV: not referenced by the vendored
 *   source subset; only ENABLE_NLS itself gates include/misc.h's `_()` macro.
 * - HAVE_VISIBILITY_ATTRIBUTE: no -fvisibility=hidden; the same compiled objects
 *   back both the static (:acl) and dynamically-linked (:acl_shared) variants.
 * - HAVE_OPENAT2 / HAVE_LINUX_OPENAT2_H / USE_OPENAT2 / UNSAFE_RESTORE_WARNINGS,
 *   HAVE_GETXATTRAT / HAVE_SETXATTRAT / HAVE_LISTXATTRAT / HAVE_REMOVEXATTRAT:
 *   only used by the "_at" fd-relative source files, which are excluded from
 *   the vendored source set because score/os/acl_impl.cpp never calls them.
 * - WORDS_BIGENDIAN: all supported targets (x86_64, aarch64) are little-endian.
 * - _FILE_OFFSET_BITS / _LARGE_FILES / _TIME_BITS: off_t/time_t are already
 *   64-bit by default on the 64-bit-only targets this repo supports.
 */

#endif /* SCORE_BASELIBS_THIRD_PARTY_ACL_CONFIG_H */
