#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
RESULT_FILE="${SCRIPT_DIR}/memcheck-unit-tests.txt"
SUMMARY_FILE="${SCRIPT_DIR}/memcheck-summary.txt"
QT_PREFIX="${QT_PREFIX:-/home/kalu/programs/qt/6.8.0/gcc_64}"

rm -rf "${BUILD_DIR}"

{
  echo "valgrind: $(valgrind --version)"
  echo "cmake: $(cmake --version | head -n 1)"
  echo "c++: $(${CXX:-c++} --version | head -n 1)"
  echo "Qt prefix: ${QT_PREFIX}"
  echo
  echo "Configuring unit test build for Memcheck..."
} | tee "${RESULT_FILE}"

cmake -S "${REPO_ROOT}/unit_tests" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="${QT_PREFIX}" \
  -DENABLE_COVERAGE=OFF \
  2>&1 | tee -a "${RESULT_FILE}"

echo | tee -a "${RESULT_FILE}"
echo "Building graphlab_unit_tests..." | tee -a "${RESULT_FILE}"

cmake --build "${BUILD_DIR}" --target graphlab_unit_tests -j"$(nproc)" \
  2>&1 | tee -a "${RESULT_FILE}"

echo | tee -a "${RESULT_FILE}"
echo "Running Valgrind Memcheck..." | tee -a "${RESULT_FILE}"

set +e
valgrind \
  --tool=memcheck \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  "${BUILD_DIR}/graphlab_unit_tests" -txt \
  2>&1 | tee -a "${RESULT_FILE}"
status=${PIPESTATUS[0]}
set -e

{
  echo "Memcheck exit status: ${status}"
  echo
  awk '/LEAK SUMMARY:/,/ERROR SUMMARY:/' "${RESULT_FILE}" || true
} | tee "${SUMMARY_FILE}"

exit "${status}"
