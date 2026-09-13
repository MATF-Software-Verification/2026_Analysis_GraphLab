#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
RESULTS_DIR="${SCRIPT_DIR}/results"
REPORT_FILE="${RESULTS_DIR}/cppcheck-report.txt"
SUMMARY_FILE="${RESULTS_DIR}/cppcheck-summary.txt"
CPPCHECK_BIN="${CPPCHECK_BIN:-cppcheck}"

if ! command -v "${CPPCHECK_BIN}" >/dev/null 2>&1; then
  echo "Cppcheck executable not found: ${CPPCHECK_BIN}" >&2
  echo "Install it with: sudo apt install cppcheck" >&2
  exit 1
fi

mkdir -p "${RESULTS_DIR}"
cd "${REPO_ROOT}"

{
  echo "cppcheck: $(${CPPCHECK_BIN} --version)"
  echo "scope: GraphLab/src (production C/C++ source)"
  echo "enabled checks: error, warning, portability"
  echo "inconclusive findings: enabled"
  echo
} | tee "${REPORT_FILE}"

set +e
"${CPPCHECK_BIN}" \
  --enable=warning,portability \
  --inconclusive \
  --std=c++17 \
  --language=c++ \
  --library=qt \
  --suppress=missingIncludeSystem \
  --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
  --quiet \
  --force \
  -iGraphLab/src/build \
  -iGraphLab/src/graph/graph.test.cpp \
  GraphLab/src \
  2>&1 | tee -a "${REPORT_FILE}"
status=${PIPESTATUS[0]}
set -e

finding_count=$(awk '/\[(.*)\]$/ { count++ } END { print count + 0 }' "${REPORT_FILE}")

{
  echo "Cppcheck exit status: ${status}"
  echo "Reported findings: ${finding_count}"
  echo
  awk '/\[(.*)\]$/ { print }' "${REPORT_FILE}"
} | tee "${SUMMARY_FILE}"

exit "${status}"
