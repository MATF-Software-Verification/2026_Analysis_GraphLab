#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
RESULTS_DIR="${SCRIPT_DIR}/results"
QT_PREFIX="${QT_PREFIX:-/home/kalu/programs/qt/6.8.0/gcc_64}"
PERF_BIN="${PERF_BIN:-perf}"

if ! command -v "${PERF_BIN}" >/dev/null 2>&1; then
  echo "perf executable not found: ${PERF_BIN}" >&2
  exit 1
fi

if [[ "${PERF_USE_SUDO:-0}" == "1" ]]; then
  perf_command=(sudo "${PERF_BIN}")
else
  perf_command=("${PERF_BIN}")
fi

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}" "${RESULTS_DIR}"

{
  echo "perf: $(${PERF_BIN} --version)"
  echo "cmake: $(cmake --version | head -n 1)"
  echo "c++: $(${CXX:-c++} --version | head -n 1)"
  echo "Qt prefix: ${QT_PREFIX}"
  echo "virtualization: $(systemd-detect-virt 2>/dev/null || echo none)"
  echo "perf_event_paranoid: $(cat /proc/sys/kernel/perf_event_paranoid 2>/dev/null || echo unknown)"
} | tee "${RESULTS_DIR}/environment.txt"

cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_PREFIX_PATH="${QT_PREFIX}" \
  2>&1 | tee "${RESULTS_DIR}/cmake-configure.txt"

cmake --build "${BUILD_DIR}" --target graphlab_perf_benchmark -j"$(nproc)" \
  2>&1 | tee "${RESULTS_DIR}/build.txt"

benchmark="${BUILD_DIR}/graphlab_perf_benchmark"

if ! "${perf_command[@]}" stat -e task-clock -- true >/dev/null 2>&1; then
  cat >&2 <<EOF
perf events are not available to the current user.
Run the script with sudo only for perf commands:
  PERF_USE_SUDO=1 ./perf/run_perf.sh
EOF
  exit 1
fi

run_profile() {
  local name=$1
  local nodes=$2
  local repetitions=$3
  local data_file="${BUILD_DIR}/${name}.perf.data"

  "${perf_command[@]}" stat \
    -r 3 \
    -e task-clock,cpu-clock:u,cycles,context-switches,cpu-migrations,page-faults \
    -- "${benchmark}" "${name}" "${nodes}" "${repetitions}" \
    >"${RESULTS_DIR}/${name}-stat.txt" 2>&1

  "${perf_command[@]}" record \
    -e cpu-clock:u \
    -F 199 \
    --call-graph fp \
    --output="${data_file}" \
    -- "${benchmark}" "${name}" "${nodes}" "${repetitions}" \
    >"${RESULTS_DIR}/${name}-record.txt" 2>&1

  if [[ "${PERF_USE_SUDO:-0}" == "1" ]]; then
    sudo chown "$(id -u):$(id -g)" "${data_file}"
  fi

  "${PERF_BIN}" report \
    --stdio \
    --no-children \
    --percent-limit 0.5 \
    --sort symbol,dso \
    --input="${data_file}" \
    >"${RESULTS_DIR}/${name}-report.txt"
}

run_profile dijkstra 12000 5
run_profile floyd-warshall 600 3

stat_value() {
  local file=$1
  local event=$2
  awk -v event="${event}" '$2 == event || $3 == event { print $1; exit }' "${file}"
}

elapsed_value() {
  awk '/seconds time elapsed/ { print $1; exit }' "$1"
}

dijkstra_stat="${RESULTS_DIR}/dijkstra-stat.txt"
floyd_stat="${RESULTS_DIR}/floyd-warshall-stat.txt"

{
  echo "Workloads profiled: 2"
  echo "Dijkstra: 12,000 nodes, 5 repetitions"
  echo "Floyd-Warshall: 600 nodes, 3 repetitions"
  echo "perf stat repetitions per workload: 3"
  echo "perf stat events: task-clock, cpu-clock:u, cycles, context-switches, cpu-migrations, page-faults"
  echo "Sampling event: cpu-clock:u"
  echo "Sampling frequency: 199 Hz"
  echo
  echo "Average measurements:"
  echo "  Dijkstra elapsed: $(elapsed_value "${dijkstra_stat}") s"
  echo "  Dijkstra task clock: $(stat_value "${dijkstra_stat}" task-clock) ms"
  echo "  Dijkstra cycles: $(stat_value "${dijkstra_stat}" cycles)"
  echo "  Dijkstra page faults: $(stat_value "${dijkstra_stat}" page-faults)"
  echo "  Floyd-Warshall elapsed: $(elapsed_value "${floyd_stat}") s"
  echo "  Floyd-Warshall task clock: $(stat_value "${floyd_stat}" task-clock) ms"
  echo "  Floyd-Warshall cycles: $(stat_value "${floyd_stat}" cycles)"
  echo "  Floyd-Warshall page faults: $(stat_value "${floyd_stat}" page-faults)"
  echo
  echo "Detailed results:"
  echo "  perf/results/dijkstra-stat.txt"
  echo "  perf/results/dijkstra-report.txt"
  echo "  perf/results/floyd-warshall-stat.txt"
  echo "  perf/results/floyd-warshall-report.txt"
} | tee "${RESULTS_DIR}/perf-summary.txt"
