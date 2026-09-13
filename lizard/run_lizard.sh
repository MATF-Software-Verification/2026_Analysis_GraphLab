#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
RESULTS_DIR="${SCRIPT_DIR}/results"
REPORT_FILE="${RESULTS_DIR}/lizard-report.txt"
SUMMARY_FILE="${RESULTS_DIR}/lizard-summary.txt"
PYTHON_BIN="${PYTHON_BIN:-python3}"

if ! "${PYTHON_BIN}" -c 'import lizard' >/dev/null 2>&1; then
  echo "The Lizard Python package is not installed." >&2
  echo "Install it with: ${PYTHON_BIN} -m pip install -r ${SCRIPT_DIR}/requirements.txt" >&2
  exit 1
fi

mkdir -p "${RESULTS_DIR}"
cd "${REPO_ROOT}"

mapfile -d '' sources < <(
  find GraphLab/src \
    -path 'GraphLab/src/build' -prune -o \
    -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) \
    ! -name 'graph.test.cpp' -print0
)

set +e
"${PYTHON_BIN}" -m lizard \
  -l cpp \
  -C 10 \
  -T nloc=50 \
  -a 5 \
  -V \
  "${sources[@]}" \
  2>&1 | tee "${REPORT_FILE}"
status=${PIPESTATUS[0]}
set -e

if [[ "${status}" -gt 1 ]]; then
  echo "Lizard failed with exit status ${status}." >&2
  exit "${status}"
fi

warning_count=$(awk '/^[[:space:]]+[0-9]+[[:space:]]/ { value=$6 } END { print value + 0 }' "${REPORT_FILE}")

{
  echo "lizard: $("${PYTHON_BIN}" -m lizard --version)"
  echo "Lizard exit status: ${status}"
  echo "Threshold warnings: ${warning_count}"
  echo "Thresholds: CCN > 10, NLOC > 50, parameters > 5"
  echo
  awk '/^!!!! Warnings/ { copy=1 } copy { print }' "${REPORT_FILE}"
} | tee "${SUMMARY_FILE}"

# Exit status 1 means that one or more configured thresholds were exceeded.
exit 0
