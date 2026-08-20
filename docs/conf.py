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


project = "S-CORE Baselibs"
project_url = "https://eclipse-score.github.io/baselibs"
version = "0.1"
extensions = [
    "sphinxcontrib.plantuml",
    "score_sphinx_bundle",
]

required_in_id = ["baselibs"]

# Not part of the Sphinx doc tree; it's a plain-Markdown style guide referenced
# directly (e.g. from AGENTS.md), not meant to be rendered as a docs page.
exclude_patterns = ["cpp-style-guide.md"]
