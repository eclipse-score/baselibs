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

"""Repository rule exposing a self-contained GCC sysroot for the LLVM toolchain.

The score GCC package extracts to a GCC install prefix whose target tool directory
`<triple>` holds a bare glibc sysroot (`<triple>/sysroot`) and the libstdc++ headers
(`<triple>/include/c++`), while the GCC support libraries (`lib/gcc`) live at the
prefix root. Clang only searches those under `--sysroot` at the standard
`usr/include/c++` and `usr/lib/gcc` locations, so this rule relocates them into the
sysroot to make it self-contained. Clang then discovers the GCC installation, the
libstdc++ headers and the CRT files automatically from `--sysroot` alone, with no
extra flags.

The sysroot directory is exposed as its own Bazel package
(`//<triple>/sysroot:sysroot`) because the LLVM toolchain derives its `--sysroot`
path from the package of the label passed to `llvm.sysroot`.

Only the outermost directory is stripped during extraction (matching the score GCC
package); stripping deeper breaks the relative tool symlinks under `bin/`.
"""

_SYSROOT_BUILD = """\
filegroup(
    name = "sysroot",
    srcs = glob(["**"], allow_empty = True),
    visibility = ["//visibility:public"],
)
"""

def _gcc_sysroot_impl(rctx):
    rctx.download_and_extract(
        url = rctx.attr.url,
        sha256 = rctx.attr.sha256,
        stripPrefix = rctx.attr.strip_prefix,
    )
    triple = rctx.attr.triple
    sysroot = "{}/sysroot".format(triple)

    # Relocate the libstdc++ headers (`<triple>/include/c++`) and the GCC support
    # libraries (`lib/gcc`), which the package keeps outside the sysroot, into it at
    # the standard `usr/include/c++` and `usr/lib/gcc` locations so Clang finds them
    # via --sysroot without any additional flags. rctx.rename creates parent
    # directories as needed.
    rctx.rename("{}/include/c++".format(triple), "{}/usr/include/c++".format(sysroot))
    rctx.rename("lib/gcc", "{}/usr/lib/gcc".format(sysroot))

    rctx.file("{}/BUILD".format(sysroot), _SYSROOT_BUILD)

gcc_sysroot = repository_rule(
    implementation = _gcc_sysroot_impl,
    attrs = {
        "sha256": attr.string(doc = "SHA-256 checksum of the tarball."),
        "strip_prefix": attr.string(
            doc = "Prefix stripped during extraction (only the outermost directory).",
        ),
        "triple": attr.string(
            mandatory = True,
            doc = "Target triple directory holding `sysroot`, `include` and `lib` (e.g. x86_64-unknown-linux-gnu).",
        ),
        "url": attr.string(mandatory = True, doc = "URL of the GCC package tarball."),
    },
)
