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

"""
Rule for creating a QEMU user-mode emulation wrapper script.

QEMU user-mode emulation allows running Linux executables compiled for one CPU
architecture on a host with a different CPU architecture (e.g. running aarch64
binaries on an x86_64 host). The generated script is intended to be used with
Bazel's --run_under flag to transparently execute cross-compiled test binaries.

The sysroot path is passed to QEMU via the -L flag so that the emulator can
locate the target architecture's shared libraries at runtime.

Prerequisites:
    The QEMU user-mode emulator for the target architecture must be installed on
    the host system and available on $PATH. For example, to emulate aarch64
    binaries on Debian/Ubuntu, install the ``qemu-user`` package.
"""

load("@rules_cc//cc/common:cc_common.bzl", "cc_common")

def _qemu_emulator_wrapper_impl(ctx):
    # Get sysroot from explicit attr or from the CC toolchain
    if ctx.attr.sysroot:
        sysroot = ctx.attr.sysroot[DefaultInfo].files.to_list()[0].path
    else:
        cc_toolchain = ctx.attr._cc_toolchain[cc_common.CcToolchainInfo]
        sysroot = cc_toolchain.sysroot

    script = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = script,
        content = """#!/bin/bash
set -euo pipefail
# Resolve symlinks to find the real execroot, then locate the sysroot.
REAL_SCRIPT="$(readlink -f "$0")"
EXECROOT="${{REAL_SCRIPT%/bazel-out/*}}"
exec qemu-{arch} -L "$EXECROOT/{sysroot}" "$@"
""".format(
            arch = ctx.attr.arch,
            sysroot = sysroot,
        ),
        is_executable = True,
    )

    return [DefaultInfo(executable = script)]

qemu_emulator_wrapper = rule(
    implementation = _qemu_emulator_wrapper_impl,
    attrs = {
        "arch": attr.string(
            mandatory = True,
            doc = "Target CPU architecture (e.g. 'aarch64').",
        ),
        "sysroot": attr.label(
            mandatory = False,
            doc = "Label pointing to the sysroot directory that is passed to QEMU. If not provided, uses the CC toolchain's sysroot.",
        ),
        "_cc_toolchain": attr.label(
            default = Label("@bazel_tools//tools/cpp:current_cc_toolchain"),
        ),
    },
    executable = True,
    doc = "Generates a wrapper script for running tests under qemu with the correct sysroot.",
)

def qemu_aarch64(name = "qemu_aarch64", sysroot = None, **kwargs):
    """Convenience macro for creating a QEMU aarch64 emulator wrapper.

    Args:
        name: Optional name of the wrapper target.
        sysroot: Optional sysroot label.
        **kwargs: Additional arguments passed to qemu_emulator_wrapper.
    """
    qemu_emulator_wrapper(
        name = name,
        arch = "aarch64",
        sysroot = sysroot,
        **kwargs
    )
