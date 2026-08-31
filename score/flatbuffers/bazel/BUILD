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

load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

py_binary(
    name = "inject_buffer_version",
    srcs = ["inject_buffer_version.py"],
    # "manual" excludes this target from wildcard builds (e.g. //score/...) so it is not
    # built directly for a cross-compilation target platform. It is only ever built for the
    # exec (host) platform when referenced as a tool via cfg = "exec" in the starlark rules.
    tags = ["manual"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "schema_generator_lib",
    srcs = ["schema_generator.py"],
    # Put this package on sys.path so the test can "import schema_generator" directly,
    # matching how the script is imported when run as a standalone tool.
    imports = ["."],
    # "manual" excludes this target from wildcard builds so it is only built for the exec
    # (host) platform when referenced as a tool via cfg = "exec" in the starlark rules.
    tags = ["manual"],
)

py_binary(
    name = "schema_generator",
    srcs = ["schema_generator.py"],
    # "manual" excludes this target from wildcard builds so it is only built for the exec
    # (host) platform when referenced as a tool via cfg = "exec" in the starlark rules.
    tags = ["manual"],
    visibility = ["//visibility:public"],
)

py_test(
    name = "schema_generator_test",
    size = "small",
    srcs = ["schema_generator_test.py"],
    deps = [":schema_generator_lib"],
)
