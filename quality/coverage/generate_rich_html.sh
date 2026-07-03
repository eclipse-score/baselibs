#!/usr/bin/env bash
# Generate a rich, git-annotated HTML coverage report with genhtml (lcov 2.4).
#
# Produces navigation links (hit/missed jumps), per-line git blame annotations
# (author, commit id, date), owner/date summary tables and a hierarchical layout.
#
# Usage:
#   quality/coverage/generate_rich_html.sh [input.json] [output-dir]
#
# Defaults:
#   input.json  = cpp_coverage_extract/json_report/coverage.json
#   output-dir  = cpp_coverage_rich
#
# The llvm-cov JSON is first converted to an lcov .info file via llvm2lcov,
# then rendered to HTML with genhtml.
#
# Before running the script, export LCOV environment variable to point to your lcov-2.4 root directory.
#

set -euo pipefail


GENHTML="${LCOV}/bin/genhtml"
LLVM2LCOV="${LCOV}/bin/llvm2lcov"

JSON_INPUT="${1:-cpp_coverage_extract/json_report/coverage.json}"
OUTPUT_DIR="${2:-cpp_coverage_rich}"

# Optional differential ("diff") coverage view.
#   BASELINE_INFO   - path to a baseline .info file (coverage of the old version).
#   BASELINE_COMMIT - git ref to diff the current sources against (e.g. origin/main).
#   DIFF_FILE       - path to write/read the unified diff (default: <output>.diff).
# The diff view activates only when BOTH BASELINE_INFO and BASELINE_COMMIT are set.
BASELINE_INFO="${BASELINE_INFO:-}"
BASELINE_COMMIT="${BASELINE_COMMIT:-}"
DIFF_FILE="${DIFF_FILE:-${OUTPUT_DIR}.diff}"

# The .info file is derived from the JSON input path (same dir, .info extension).
INFO_FILE="${JSON_INPUT%.json}.info"

if [[ ! -x "${GENHTML}" ]]; then
  echo "ERROR: genhtml not found at ${GENHTML}. Set LCOV to your lcov-2.4 root." >&2
  exit 1
fi

if [[ ! -x "${LLVM2LCOV}" ]]; then
  echo "ERROR: llvm2lcov not found at ${LLVM2LCOV}. Set LCOV to your lcov-2.4 root." >&2
  exit 1
fi

if [[ ! -f "${JSON_INPUT}" ]]; then
  echo "ERROR: coverage JSON not found: ${JSON_INPUT}" >&2
  exit 1
fi

# Convert the llvm-cov JSON export to an lcov .info file.
# Test sources and googletest internals leak into the JSON's functions[] array
# (llvm-cov does not filter them there), so exclude them to avoid fatal
# "unknown file" errors in llvm2lcov.
"${LLVM2LCOV}" \
  --output "${INFO_FILE}" \
  --branch-coverage \
  --exclude '*_test.cpp' \
  --exclude '*/external/googletest+/*' \
  --exclude '*/external/openssl+/*' \
  --exclude '*mock.h' \
  --exclude '*mock.cpp' \
  --exclude '*mock_*' \
  --exclude '*/test/*' \
  --exclude '*/unit_test/*' \
  --ignore-errors inconsistent,unsupported \
  "${JSON_INPUT}"

# Assemble optional differential coverage arguments.
DIFF_ARGS=()
if [[ -n "${BASELINE_INFO}" && -n "${BASELINE_COMMIT}" ]]; then
  if [[ ! -f "${BASELINE_INFO}" ]]; then
    echo "ERROR: baseline coverage file not found: ${BASELINE_INFO}" >&2
    exit 1
  fi
  # Generate a unified diff (baseline sources -> current working tree) using the
  # shipped gitdiff helper so paths match the coverage data exactly.
  echo "Generating diff ${BASELINE_COMMIT}..HEAD -> ${DIFF_FILE}"
  "${LCOV}/scripts/gitdiff" "${BASELINE_COMMIT}" HEAD > "${DIFF_FILE}"
  DIFF_ARGS=(
    --baseline-file "${BASELINE_INFO}"
    --diff-file "${DIFF_FILE}"
  )
fi

"${GENHTML}" \
  "${INFO_FILE}" \
  --output-directory "${OUTPUT_DIR}" \
  --branch-coverage \
  --show-navigation \
  --demangle-cpp \
  --annotate-script "${LCOV}/scripts/gitblame.pm" \
  --version-script  "${LCOV}/scripts/gitversion.pm" \
  --legend \
  --title "Baselibs coverage" \
  --header-title "Baselibs (git-annotated)" \
  --sort-tables \
  --flat \
  ${DIFF_ARGS[@]+"${DIFF_ARGS[@]}"} \
  -j 4 \
  --ignore-errors inconsistent,unsupported,annotate,version,source,category,path,empty,mismatch

# Add to to filter out lines marked with LCOV_EXCL_*
#   --filter region,branch_region \

echo "Rich coverage report written to: ${OUTPUT_DIR}/index.html"
