#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
RESULTS_DIR="${SCRIPT_DIR}/results"
REPORT_FILE="${RESULTS_DIR}/clang-format-report.txt"
SUMMARY_FILE="${RESULTS_DIR}/clang-format-summary.txt"
COMPARISON_FILE="${RESULTS_DIR}/style-comparison.txt"
CLANG_FORMAT_BIN="${CLANG_FORMAT_BIN:-clang-format}"
SELECTED_STYLE="Microsoft"

if ! command -v "${CLANG_FORMAT_BIN}" >/dev/null 2>&1; then
  echo "clang-format executable not found: ${CLANG_FORMAT_BIN}" >&2
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

{
  echo "clang-format: $(${CLANG_FORMAT_BIN} --version)"
  printf '%-12s %-14s %s\n' "STYLE" "CHANGED FILES" "DIAGNOSTICS"

  for style in LLVM Google Chromium Mozilla WebKit Microsoft GNU; do
    style_diagnostics=0
    changed_files=0

    for source in "${sources[@]}"; do
      set +e
      output=$("${CLANG_FORMAT_BIN}" \
        --dry-run \
        --Werror \
        --ferror-limit=0 \
        --style="${style}" \
        "${source}" 2>&1)
      status=$?
      set -e

      diagnostics=$(awk '/clang-format-violations/ { count++ } END { print count + 0 }' <<<"${output}")
      if [[ "${status}" -ne 0 && "${diagnostics}" -eq 0 ]]; then
        echo "clang-format failed while comparing ${style} on ${source}:" >&2
        echo "${output}" >&2
        exit "${status}"
      fi

      if [[ "${diagnostics}" -gt 0 ]]; then
        ((changed_files += 1))
        ((style_diagnostics += diagnostics))
      fi
    done

    printf '%-12s %-14d %d\n' "${style}" "${changed_files}" "${style_diagnostics}"
  done
} | tee "${COMPARISON_FILE}"

{
  echo "clang-format: $(${CLANG_FORMAT_BIN} --version)"
  echo "style: ${SELECTED_STYLE}"
  echo "scope: GraphLab/src (production C/C++ source)"
  echo
  printf '%-12s %s\n' "DIAGNOSTICS" "FILE"
} >"${REPORT_FILE}"

compliant=0
noncompliant=0
total_diagnostics=0

for source in "${sources[@]}"; do
  set +e
  output=$("${CLANG_FORMAT_BIN}" \
    --dry-run \
    --Werror \
    --ferror-limit=0 \
    --style="${SELECTED_STYLE}" \
    "${source}" 2>&1)
  status=$?
  set -e

  if [[ "${status}" -eq 0 ]]; then
    ((compliant += 1))
    continue
  fi

  diagnostics=$(awk '/clang-format-violations/ { count++ } END { print count + 0 }' <<<"${output}")
  if [[ "${diagnostics}" -eq 0 ]]; then
    echo "clang-format failed while checking ${source}:" >&2
    echo "${output}" >&2
    exit "${status}"
  fi

  ((noncompliant += 1))
  ((total_diagnostics += diagnostics))
  printf '%-12d %s\n' "${diagnostics}" "${source}" >>"${REPORT_FILE}"
done

{
  echo "clang-format: $(${CLANG_FORMAT_BIN} --version)"
  echo "Style: ${SELECTED_STYLE}"
  echo "Files checked: ${#sources[@]}"
  echo "Conforming files: ${compliant}"
  echo "Non-conforming files: ${noncompliant}"
  echo "Formatting diagnostics: ${total_diagnostics}"
  echo "Source files modified: 0"
} | tee "${SUMMARY_FILE}"

cat "${REPORT_FILE}"
