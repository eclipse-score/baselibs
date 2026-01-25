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

# clang-tidy will check every source file of the current bazel package and emit diagnostics only within such package's header files
def clang_tidy_extra_checks(name, tags = [], extra_features = [], tidy_config_file = None, skip_testonly_targets = False):
    # this is just a dummy action for now as long as user-defined extra checks via clang-tidy are not supported
    native.genrule(
        name = name,
        deprecation = "NOT YET SUPPORTED",
        cmd = "echo 'NOT YET SUPPORTED' > $(@)",
        executable = False,
        tags = ["manual"] + tags,
        outs = [name + ".result"],
    )

