# perf CPU Profiling

This directory contains Linux perf results for GraphLab algorithms.

## Dependencies

- Linux perf compatible with the running kernel
- CMake 3.16 or newer
- a C++17 compiler
- Qt 6 Core
- Bash

On Ubuntu, perf can be installed with:

```bash
sudo apt install linux-tools-common linux-tools-$(uname -r)
```

## Running the Analysis

From the repository root, run:

```bash
./perf/run_perf.sh
```

The script builds `graphlab_perf_benchmark` in `RelWithDebInfo` mode with frame
pointers enabled. It uses the original GraphLab implementations.

Two deterministic workloads are used:

- Dijkstra on a directed weighted graph with 12,000 nodes, repeated 5 times;
- Floyd-Warshall on a directed weighted graph with 600 nodes, repeated 3 times.

Dijkstra and Floyd-Warshall represent different workloads. Dijkstra finds paths
from one source on a large sparse graph and records steps for visualization.
Floyd-Warshall calculates paths between all pairs and creates large result
matrices.

Each graph contains a chain and fixed forward shortcuts. The benchmark prints a
checksum so that results can be compared between runs.

`perf stat` runs each workload three times. It records CPU time, CPU cycles,
context switches, CPU migrations, and page faults. `perf record` samples user
CPU time at 199 Hz. `perf report` lists functions with at least 0.5% overhead.

The analysis was run in VMware. Instruction, branch, and cache counters returned
zero in this environment, so they were not used.

## Results

- `results/environment.txt` contains tool and system information.
- `results/*-stat.txt` contains counter measurements.
- `results/*-record.txt` contains output from the sampling run.
- `results/*-report.txt` contains CPU usage by function.
- `results/perf-summary.txt` contains the workloads and average measurements.

The build directory and binary `perf.data` files are ignored by Git. The text
reports are kept as analysis results.

### Measurements

| Workload | Average elapsed time | Average task clock | Average cycles | Page faults |
| --- | ---: | ---: | ---: | ---: |
| Dijkstra | 2.811 s | 2,642.34 ms | 7,641,854,727 | 5,032 |
| Floyd-Warshall | 1.106 s | 1,103.69 ms | 3,188,566,971 | 23,280 |

### CPU Hotspots

| Workload | Hotspot | Overhead | Assessment | Proposed action |
| --- | --- | ---: | --- | --- |
| Dijkstra | Edge lookup through `std::unordered_map::at` | 55.05% | The same edge data is read from the map many times. | Reuse edge data inside the loop. |
| Dijkstra | `Dijkstra::reconstruct_path` | 37.36% | The full path is rebuilt for every saved step. | Save less data and rebuild the path only for display. |
| Floyd-Warshall | `FloydWarshall::run` | 56.68% | Most time is spent in the expected matrix calculation. | No change needed. |
| Floyd-Warshall | `_int_malloc` | 16.13% | The path matrix creates many small vectors. | Consider storing only the next edge for each node pair. |
| Floyd-Warshall | Nested-vector construction | 8.29% | Some of this work comes from copying the result matrices. | Return the existing matrices without copying them. |

Saving full paths for Dijkstra visualization steps causes most of its measured
cost. In Floyd-Warshall, `return std::make_pair(distances, paths)` copies both
matrices. The return statement can reuse their existing memory instead.
