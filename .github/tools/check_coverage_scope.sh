#!/usr/bin/env bash
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
# Coverage scope completeness check.
#
# Compares three sets:
#   1. every production (non-testonly, non-QNX-only) cc_library / rust_library
#      under //score/... (the universe),
#   2. everything transitively reachable from //tools/coverage:coverage_scope,
#   3. the justified exclusions in tools/coverage/scope_exclusions.txt.
#
# Fails when a universe target is neither reachable from the scope nor
# excluded (coverage would silently miss its files), and when an exclusion
# has become reachable (stale entry).
set -euo pipefail
cd "$(dirname "$0")/../.."

SCOPE=//tools/coverage:coverage_scope
UNIVERSE='//score/...'
EXCLUSIONS=tools/coverage/scope_exclusions.txt

all=$(mktemp) scoped=$(mktemp) excluded=$(mktemp)
trap 'rm -f "$all" "$scoped" "$excluded"' EXIT

# Plain query (not cquery): the universe must not be configured - the repo
# contains deliberately contradictory config_setting stubs (e.g.
# generic_trace_library/flags:require_typed_memory) that abort analysis.
bazel query \
  "kind(\"cc_library|rust_library\", $UNIVERSE) \
   except attr(testonly, 1, $UNIVERSE) \
   except attr(\"target_compatible_with\", \"qnx\", $UNIVERSE)" \
  --output=label 2>/dev/null | sort -u > "$all"

bazel cquery --noimplicit_deps \
  "kind(\"cc_library|rust_library\", deps(labels(deps, $SCOPE)))" \
  --output=label 2>/dev/null | awk '{print $1}' | grep '^//score' | sort -u > "$scoped"

(grep -oE '^//[^ #]+' "$EXCLUSIONS" || true) | sort -u > "$excluded"

missing=$(comm -23 "$all" "$scoped" | comm -23 - "$excluded")
stale=$(comm -12 "$scoped" "$excluded")

status=0
if [ -n "$missing" ]; then
  echo "ERROR: production targets neither in the coverage scope nor excluded:"
  echo "$missing" | sed 's/^/  /'
  echo "Add them to $SCOPE deps, or to $EXCLUSIONS with a justification."
  status=1
fi
if [ -n "$stale" ]; then
  echo "ERROR: exclusions that are (now) reachable from the coverage scope - remove them:"
  echo "$stale" | sed 's/^/  /'
  status=1
fi
[ "$status" -eq 0 ] && echo "Coverage scope is complete: every production target is in scope or explicitly excluded."
exit "$status"
