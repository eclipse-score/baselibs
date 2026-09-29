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

load("@score_docs_as_code//:docs.bzl", "docs_bundle")

docs_bundle(
    name = "docs",
    code_targets = [
        "//score/language/safecpp/aborts_upon_exception:abortsuponexception",
        "//score/language/safecpp/safe_math:safe_math",
        "//score/language/safecpp/safe_atomics:try_atomic_add",
        "//score/language/safecpp/scoped_function:scope",
        "//score/language/safecpp/scoped_function:copyable_scoped_function",
        "//score/language/safecpp/scoped_function:move_only_scoped_function",
        "//score/language/safecpp/string_view:zstring_view",
    ],
    primary_need_id = "comp__baselibs_safecpp",
    source_dir = "docs",
    visibility = ["//visibility:public"],
)
