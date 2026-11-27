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

# score_tooling is a dev dependency of score_baselibs and is not visible to consumers.
# Keep its load out of //score/mw/log, which consumers load, to avoid breaking their builds.
load("@score_tooling//bazel/rules/rules_score:rules_score.bzl", "architectural_design")

# Only the diagrams that the strict PlantUML parser of score_tooling accepts.
# The other diagrams in docs/detailed_design/_assets need to be rewritten first:
# https://github.com/eclipse-score/logging/issues/283
architectural_design(
    name = "mw_log_architectural_design",
    dynamic = [
        "//score/mw/log:docs/detailed_design/_assets/configuration_sequence.puml",
        "//score/mw/log:docs/detailed_design/_assets/wait_free_stack_trypush.puml",
    ],
    visibility = ["//visibility:public"],
)
