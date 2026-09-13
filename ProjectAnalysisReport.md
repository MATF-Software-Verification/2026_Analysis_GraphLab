# Project Analysis Report

## 1. General Information

This report documents the software verification analysis of GraphLab performed
for the Software Verification course at the Faculty of Mathematics, University
of Belgrade.

|  |  |
| --- | --- |
| Author | Luka Matic |
| Index | 1014/2025 |
| Analysis repository | `2026_Analysis_GraphLab` |
| Original project | [GraphLab](https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2024-2025/GraphLab) |
| Analyzed branch | `main` |
| Analyzed commit | `ad54f25c8f771458062385bcb2022c168b706aac` |
| Project reference | Git submodule in `GraphLab/` |


## 2. Project Description

GraphLab is a C++17 and Qt desktop application for constructing, displaying,
editing, saving, and loading graphs. It supports weighted and unweighted,
directed and undirected graphs, including multigraphs. Algorithms can be
executed as a sequence of steps that can be displayed by the graphical user
interface.

The implemented algorithms include:

- breadth-first search (BFS);
- depth-first search (DFS);
- Dijkstra's shortest-path algorithm;
- the A* shortest-path algorithm;
- the Floyd-Warshall all-pairs shortest-path algorithm;
- Prim's minimum spanning tree algorithm;
- Kruskal's minimum spanning tree algorithm;
- Eulerian path and circuit detection.

The source tree separates several responsibilities:

- `GraphLab/src/graph` contains graph, node, and edge representations;
- `GraphLab/src/algorithm` contains graph algorithm implementations;
- `GraphLab/src/Serialization` contains graph persistence logic;
- `GraphLab/src/UI` and `mainwindow.*` contain the graphical interface;
- `GraphLab/src/Synchronization` contains worker thread classes;
- `GraphLab/src/utils` and `GraphLab/src/help` contain supporting GUI logic.

## 3. Analysis Goals and Scope

The goal is to look for correctness defects, memory errors, and performance
bottlenecks by applying multiple verification tools and techniques.


The following components are outside the current test and Memcheck scope:

- main-window and graph-editor interactions;
- drawing scene and widget behavior;
- synchronization and worker thread behavior;
- help-window behavior;
- integration of all components in a complete interactive GUI session.


## 4. Environment

The committed results were produced with the following environment:

| Component | Version |
| --- | --- |
| Operating system | Ubuntu 20.04 |
| CMake | 3.16.3 |
| C++ compiler | GCC 10.5.0 |
| C++ standard | C++17 |
| Qt | 6.8.0 |
| lcov | 1.14 |
| Valgrind | 3.15.0 |

## 5. Reports for Applied Tools

### 5.1 QtTest Unit Testing and lcov Coverage

#### 5.1.1 Motivation

The project is implemented with Qt, so QtTest was selected as the unit testing
framework. It integrates with the existing Qt types and build environment.
Tests are built in a separate CMake project under `unit_tests/`. This project
compiles the required source files directly from the GraphLab submodule.

#### 5.1.2 Test Scope

The suite contains 24 explicit test methods grouped into three source files:

- `graph_unit_tests.cpp` verifies node and edge insertion, adjacency data,
  removal operations, metadata updates, QVariant conversion, and negative-cycle
  detection;
- `algorithm_unit_tests.cpp` verifies BFS, DFS, Dijkstra, A*, Floyd-Warshall,
  Prim, Kruskal, Euler logic, and movement through algorithm iterations;
- `serialization_unit_tests.cpp` verifies saving, loading, round trips, graph
  type flags, and handling of a missing input file.

QtTest also reports implicit suite initialization and cleanup, which is why the
final output contains 26 passed checks for 24 explicit test methods.

#### 5.1.3 Reproduction

From the repository root, run:

```bash
./unit_tests/run_tests.py
```

If Qt is installed at a different location, run:

```bash
QT_PREFIX=/path/to/Qt/6.x/gcc_64 ./unit_tests/run_tests.py
```

The script performs a clean CMake configuration, builds the test executable,
runs it through CTest and QtTest, captures coverage with lcov, removes system,
Qt, test, and build paths from the coverage data, and generates an HTML report.

Detailed reproduction instructions are available in
`unit_tests/RunningTests.md`.

#### 5.1.4 Results

The QtTest result is:

```text
Totals: 26 passed, 0 failed, 0 skipped, 0 blacklisted
```

The filtered coverage result is:

```text
Line coverage:     93.6% (641 of 685 lines)
Function coverage: 85.8% (103 of 120 functions)
```

The HTML report is stored at `unit_tests/coverage/html/index.html`. Text output
from configuration, compilation, CTest, and QtTest is stored in
`unit_tests/test-results/`.


### 5.2 Valgrind Memcheck

#### 5.2.1 Motivation and Scope

Memcheck was selected as the single Valgrind tool for this analysis. It checks
executed code for invalid memory access, use of undefined values, incorrect
deallocation, and memory leaks.

Memcheck is run over the unit test executable. This gives it deterministic
coverage of the graph model, algorithms, and serialization while ensuring that
the analyzed process terminates automatically.

#### 5.2.2 Reproduction

From the repository root, run:

```bash
./valgrind/memcheck/run_memcheck.sh
```

For a different Qt installation, run:

```bash
QT_PREFIX=/path/to/Qt/6.x/gcc_64 ./valgrind/memcheck/run_memcheck.sh
```

The script creates a clean Debug build without coverage instrumentation and
runs the test executable using these important Memcheck options:

- `--leak-check=full` requests detailed leak analysis;
- `--show-leak-kinds=all` displays every leak classification;
- `--track-origins=yes` helps identify the origin of undefined values;
- `--error-exitcode=1` makes detected Memcheck errors fail the script.

Detailed instructions are available in `valgrind/memcheck/Memcheck.md`.

#### 5.2.3 Results

The unit tests still pass while running under Memcheck. The latest committed
summary is:

```text
Memcheck exit status: 0
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
possibly lost: 0 bytes in 0 blocks
still reachable: 19,212 bytes in 19 blocks
ERROR SUMMARY: 0 errors from 0 contexts
```

The full log is stored in
`valgrind/memcheck/memcheck-unit-tests.txt`, while the concise result is stored
in `valgrind/memcheck/memcheck-summary.txt`.
