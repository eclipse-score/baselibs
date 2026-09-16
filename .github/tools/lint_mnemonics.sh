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
# Maps each linter's short name to the rules_lint aspect mnemonic used in its
# output filenames. Sourced by lint.yml so both steps that need this mapping
# stay in sync.
declare -A LINT_MNEMONICS=(
  [clang-tidy]=AspectRulesLintClangTidy
  [clippy]=AspectRulesLintClippy
  [ruff]=AspectRulesLintRuff
)
