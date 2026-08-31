#!/usr/bin/env python3
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
"""Checks that config.h accounts for every autoconf-style macro the vendored acl
sources reference.

config.h (third_party/acl/config.h) is a hand-written stand-in for acl's
autoconf-generated include/config.h.
Each `#define`d macro, and each intentionally-left-undefined macro, is documented
there with a comment explaining why.
This script scans the vendored acl-2.4.0 sources for `#if`/`#ifdef`/`#ifndef`/`#elif`
references to autoconf-style feature macros (HAVE_*, ENABLE_*, USE_*, PACKAGE_*, ...)
and fails if any of them is not mentioned anywhere in config.h, so a future acl
version bump or file-list change can't silently drop coverage for a macro the code
actually depends on.

This only catches macros referenced by sources but missing from config.h.
It does not flag macros defined in config.h that are no longer referenced by any
source (dead entries are low-risk and can be pruned manually during review).

It also checks that no vendored file's header claims the (non-Lesser) GNU General
Public License.
Only the LGPL-2.1(-or-later) portion of acl is meant to be vendored here (see
NOTICE); acl's GPL-2 portion is the getfacl/setfacl/chacl command-line tools, which
BUILD's ACL_SRCS/ACL_HDRS never lists, but this guards against a future edit
accidentally pulling in one of those files.
"""

import itertools
import re
import sys

MACRO_PATTERN = re.compile(
    r"\b("
    r"HAVE_[A-Z0-9_]+"
    r"|ENABLE_[A-Z0-9_]+"
    r"|USE_[A-Z0-9_]+"
    r"|PACKAGE_[A-Z0-9_]+"
    r"|UNSAFE_[A-Z0-9_]+"
    r"|WORDS_BIGENDIAN"
    r"|STDC_HEADERS"
    r"|VERSION"
    r"|_FILE_OFFSET_BITS"
    r"|_LARGE_FILES"
    r"|_TIME_BITS"
    r"|_GNU_SOURCE"
    r")\b"
)
CONDITIONAL_LINE_PATTERN = re.compile(r"^\s*#\s*(if|ifdef|ifndef|elif)\b")
GPL_PATTERN = re.compile(r"GNU General Public License")
LGPL_PATTERN = re.compile(r"Lesser General Public License|LGPL")
HEADER_LINES_TO_SCAN = 40


def is_gpl_only(source_path):
    with open(source_path, encoding="utf-8") as source_file:
        header = "".join(itertools.islice(source_file, HEADER_LINES_TO_SCAN))
    return bool(GPL_PATTERN.search(header)) and not LGPL_PATTERN.search(header)


def macros_known_to_config_h(config_h_path):
    with open(config_h_path, encoding="utf-8") as config_h:
        text = config_h.read()
    return set(MACRO_PATTERN.findall(text))


def macros_referenced_by(source_path):
    referenced = {}
    with open(source_path, encoding="utf-8") as source_file:
        for line_number, line in enumerate(source_file, start=1):
            if not CONDITIONAL_LINE_PATTERN.match(line):
                continue
            for macro in MACRO_PATTERN.findall(line):
                referenced.setdefault(macro, []).append(f"{source_path}:{line_number}")
    return referenced


def main(argv):
    if len(argv) < 2:
        print("usage: check_config_drift.py <config.h> <source>...", file=sys.stderr)
        return 2

    known_macros = macros_known_to_config_h(argv[0])

    missing = {}
    for source_path in argv[1:]:
        for macro, locations in macros_referenced_by(source_path).items():
            if macro not in known_macros:
                missing.setdefault(macro, []).extend(locations)

    gpl_only_files = [path for path in argv[1:] if is_gpl_only(path)]

    ok = True

    if missing:
        ok = False
        print(
            "config.h does not mention the following macro(s) referenced by "
            "vendored acl sources. Add a `#define` or a comment documenting why "
            "it's intentionally left undefined:",
            file=sys.stderr,
        )
        for macro in sorted(missing):
            print(f"  {macro}", file=sys.stderr)
            for location in missing[macro]:
                print(f"    referenced at {location}", file=sys.stderr)

    if gpl_only_files:
        ok = False
        print(
            "The following vendored file(s) claim the GPL (not LGPL) and must not be "
            "vendored here (see NOTICE, only acl's LGPL-2.1 portion is in scope):",
            file=sys.stderr,
        )
        for path in gpl_only_files:
            print(f"  {path}", file=sys.stderr)

    if not ok:
        return 1

    print(
        f"OK: config.h accounts for all macros referenced by {len(argv) - 1} vendored file(s), "
        "and all of them are LGPL-licensed."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
