# *******************************************************************************
# Copyright (c) 2024 Contributors to the Eclipse Foundation
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

load("@score_docs_as_code//:docs.bzl", "docs")
load("@score_tooling//:defs.bzl", "copyright_checker", "use_format_targets")

docs(
    data = [
        "@score_process//:needs_json",
    ],
    source_dir = "docs",
)

copyright_checker(
    name = "copyright",
    srcs = [
        ".github",
        "bazel",
        "docs",
        "score",
        "//:BUILD",
        "//:MODULE.bazel",
    ],
    config = "@score_tooling//cr_checker/resources:config",
    exclusion = "//:cr_checker_exclusion",
    template = "@score_tooling//cr_checker/resources:templates",
    visibility = ["//visibility:public"],
)

# Format targets - Rust targets may not work on aarch64 due to toolchain availability
use_format_targets()

# Platform-aware library aliases for backward compatibility
alias(
    name = "acl",
    actual = "//third_party/acl:acl",
    visibility = ["//visibility:public"],
)

alias(
    name = "libcap2",
    actual = "//third_party/libcap2:libcap2", 
    visibility = ["//visibility:public"],
)

alias(
    name = "libcap2-dev",
    actual = "//third_party/libcap2:libcap2",
    visibility = ["//visibility:public"],
)

alias(
    name = "libseccomp2",
    actual = "//third_party/libseccomp2:libseccomp2",
    visibility = ["//visibility:public"],
)

alias(
    name = "libseccomp2-dev", 
    actual = "//third_party/libseccomp2:libseccomp2",
    visibility = ["//visibility:public"],
)

alias(
    name = "valgrind",
    actual = "//third_party/valgrind:valgrind",
    visibility = ["//visibility:public"],
)
