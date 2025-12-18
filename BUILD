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

load("@rules_python//sphinxdocs:sphinx.bzl", "sphinx_build_binary", "sphinx_docs")
load("@rules_python//sphinxdocs:sphinx_docs_library.bzl", "sphinx_docs_library")
load("@score_tooling//bazel/rules/score_module:score_module.bzl", "safety_element_out_of_context")

docs(
    data = [
       # "@score_process//:needs_json",
    ],
    source_dir = "docs",
)



sphinx_docs(
        name = "baselibs.html",
        srcs = ["//:docs/index.rst"],
        config = "//:docs/conf.py",
        extra_opts = [
            "-W",
            "--keep-going",
            "-T",  # show more details in case of errors
            "--jobs",
            "auto",
        ],
        formats = ["html"],
        sphinx = ":sphinx_build",
        tools = [],
        visibility = ["//visibility:public"],
        # Modules export their docs as sphinx_docs_library
        # which are imported by bazel to docs/modules/<module_name>
        deps = [
            "//score/concurrency:seooc_concurrency"    
        ],
    )




