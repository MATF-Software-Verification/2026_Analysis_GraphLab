# Project Analysis Report

## 1. General Information

This report documents the software verification analysis of GraphLab performed
for the Software Verification course at the Faculty of Mathematics, University
of Belgrade.

|  |  |
| --- | --- |
| Author | Luka Matic, 1014/2025 |
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

Unit tests and Memcheck execute the graph model, algorithms, and serialization
code. Cppcheck, Lizard, and clang-format inspect all production source files,
including GUI and thread code. perf measures Dijkstra and Floyd-Warshall on
fixed benchmark graphs.

The analysis does not include automated GUI or concurrency tests. Window,
drawing, widget, help, and worker-thread behavior was inspected statically but
was not executed by the analysis.


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
| Cppcheck | 1.90 |
| Lizard | 1.21.2 |
| clang-format | 10.0.0 |
| perf | 5.15.178 |

## 5. Reports for Applied Tools

### 5.1 QtTest Unit Testing and lcov Coverage

#### 5.1.1 Motivation

QtTest was selected because GraphLab uses Qt. The tests are built by a separate
CMake project under `unit_tests/`, using source files from the GraphLab
submodule.

#### 5.1.2 Test Scope

The suite contains 24 explicit test methods grouped into three source files:

- `graph_unit_tests.cpp` tests nodes, edges, adjacency lists, removal, metadata,
  QVariant conversion, and negative-cycle detection;
- `algorithm_unit_tests.cpp` tests BFS, DFS, Dijkstra, A*, Floyd-Warshall, Prim,
  Kruskal, Euler, and movement between saved algorithm steps;
- `serialization_unit_tests.cpp` tests saving, loading, graph type flags, and a
  missing input file.

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

The script creates a clean build, runs the tests, collects lcov coverage, and
generates an HTML report. System, Qt, test, and build files are excluded from
the coverage result.

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

Memcheck was selected as the single Valgrind tool. It checks executed code for
invalid memory access, undefined values, incorrect deallocation, and memory
leaks.

It runs the unit test executable, so the same graph, algorithm, and
serialization cases are checked on every run.

#### 5.2.2 Reproduction

From the repository root, run:

```bash
./valgrind/memcheck/run_memcheck.sh
```

For a different Qt installation, run:

```bash
QT_PREFIX=/path/to/Qt/6.x/gcc_64 ./valgrind/memcheck/run_memcheck.sh
```

The script creates a clean Debug build without coverage and runs it with these
Memcheck options:

- `--leak-check=full` reports memory leaks in detail;
- `--show-leak-kinds=all` reports all leak types;
- `--track-origins=yes` traces undefined values;
- `--error-exitcode=1` fails the script when Memcheck finds an error.

Detailed instructions are available in `valgrind/memcheck/Memcheck.md`.

#### 5.2.3 Results

The unit tests pass under Memcheck. The result is:

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


### 5.3 Cppcheck

#### 5.3.1 Scope and Configuration

Cppcheck 1.90 checked 66 production C++ source and header files under
`GraphLab/src`. The check used
C++17 and Qt settings. Error, warning, portability, and inconclusive findings
were enabled. Style and performance checks were not used.

The analysis is reproduced from the repository root with:

```bash
./cppcheck/run_cppcheck.sh
```

#### 5.3.2 Results

Cppcheck reported one `uselessAssignmentPtrArg` warning in
`DrawingScene::onNodeClickedForAddEdge`. The assignment `endNode = nullptr`
changes only a local pointer just before the branch ends. The assignment is
redundant, but it does not cause a functional defect.

Cppcheck also reported an inconclusive `uninitMemberVar` warning for
`DrawingEdge::mutex`. `QMutex` is initialized by its default constructor, so
this is a false positive. No error-level or portability problems were found.

The complete output and a summary are stored in `cppcheck/results/`.


### 5.4 Lizard Complexity Analysis

#### 5.4.1 Scope and Configuration

Lizard 1.21.2 checked 66 production files. It reported functions with
cyclomatic complexity (CCN) above 10, more than 50 non-comment lines (NLOC), or
more than 5 parameters. The analysis is reproduced with:

```bash
python3 -m pip install -r lizard/requirements.txt
./lizard/run_lizard.sh
```

#### 5.4.2 Results

The code contains 4,583 lines and 259 functions. The average CCN is 2.6. Twelve
functions crossed at least one limit. Seven had high CCN, while five were
reported only for their size or number of parameters.

| Finding | Assessment | Proposed action |
| --- | --- | --- |
| `RunThread::run` (`Synchronization/runthread.cpp`) (CCN 30, NLOC 176) | **Refactoring candidate.** It runs every algorithm and changes the UI. | Move each algorithm into its own function. |
| `AlgorithmWidget::simulateIteration` (`UI/GraphEditor/Menu/Algorithm/algorthmwidget.cpp`) (CCN 20, NLOC 86) | **Refactoring candidate.** Similar navigation and drawing logic is repeated. | Separate navigation from drawing results. |
| `Helpers::obtainFloydWarshallIterationAndDisplayEffect` (`utils/helpers.cpp`) (CCN 15, NLOC 87, 6 parameters) | **Refactoring candidate.** Path selection and UI updates are combined. | Separate path selection from UI updates. |
| `MainWindow::closeEvent` (`mainwindow.cpp`) (CCN 11, NLOC 53) | **Refactoring candidate.** Finding unsaved tabs, saving, and closing are combined. | Move tab search and saving into helper functions. |
| `FloydWarshall::run` (`algorithm/Floyd_Warshall/floyd_warshall.cpp`) (CCN 21, NLOC 54) | **Acceptable.** The three nested loops are required by the algorithm. | No change needed. |
| `MST_Prim::run` (`algorithm/MST_Prim/mst_prim.cpp`) (CCN 11, NLOC 47) | **Acceptable.** Most branches are required by Prim's algorithm. | No change needed. |
| `Graph::contains_negative_cucles` (`graph/graph.cpp`) (CCN 11, NLOC 37) | **Acceptable.** The loops follow the negative-cycle detection algorithm. | No change based on this result. |
| `AlgorithmWidget::onAlgorithmChanged` (`UI/GraphEditor/Menu/Algorithm/algorthmwidget.cpp`) (CCN 9, NLOC 85) | **Acceptable.** It was reported for size, not high CCN. | No change needed. |
| `HelpAlgorithm::showHelpForAlgorithm` (`utils/helpalgorithm.cpp`) (CCN 9, NLOC 60) | **Acceptable.** It was reported for size, not high CCN. | No change needed. |
| `GraphEditor::GraphEditor` (`UI/GraphEditor/grapheditor.cpp`) (CCN 2, NLOC 79) | **Acceptable.** It is long but has simple control flow. | No change needed. |
| `DrawingNode::DrawingNode` (`UI/GraphEditor/Scene/drawingnode.cpp`) (CCN 2, NLOC 43, 7 parameters) | **Acceptable.** It was reported for its parameter count. | No change needed. |
| `MainWindow::MainWindow` (`mainwindow.cpp`) (CCN 1, NLOC 57) | **Acceptable.** It is long but has simple control flow. | No change needed. |

Full per-function metrics and the threshold summary are stored in
`lizard/results/`. Manual review identified four refactoring candidates. The
other eight warnings come from expected algorithm structure, function length,
or parameter count.


### 5.5 clang-format

#### 5.5.1 Scope and Configuration

GraphLab has no `.clang-format` file. Its indentation and brace placement are
closest to the built-in Microsoft style, so that style was used as the
reference. clang-format 10.0.0 checked 66 production files with `--dry-run
--Werror`. It did not modify the source code.

The check is reproduced with:

```bash
./clang_format/run_clang_format.sh
```

#### 5.5.2 Results

All 66 files differ from the Microsoft style in at least one place. The check
reported 2,603 differences. The highest counts were in `grapheditor.cpp` (254),
`drawingedge.cpp` (182), `mainwindow.cpp` (154), `graph.cpp` (128), and
`algorthmwidget.cpp` (114). Seventeen files had ten differences or fewer.

Each diagnostic marks code that clang-format would change. The common
differences are:

| Difference | Example | Expected Microsoft form |
| --- | --- | --- |
| Control statement braces | `graph/graph.cpp:22` keeps `{` on the same line as `for`. | Opening brace on the next line. |
| Syntax spacing | `runthread.cpp:22` uses `if(canExecute){`. | Spaces after `if` and before `{`. |
| Pointer/reference alignment | `graph/graph.cpp:7` uses `title_t& title`. | `title_t &title`. |
| Parameter spacing | `drawingedge.cpp:8` omits spaces after commas. | A space after every comma. |
| Comments and includes | Some comments omit the space after `//`, and local headers are not consistently ordered. | Spaced comments and sorted includes. |
| Long GUI statements | Several signal connections exceed 120 columns. | Wrapped and aligned statements. |

Most differences are in larger GUI files. Counts by file and the style comparison are stored in
`clang_format/results/`.


### 5.6 Linux perf

#### 5.6.1 Scope and Configuration

A separate `RelWithDebInfo` benchmark runs the original Dijkstra and
Floyd-Warshall code without changing GraphLab. It uses the same generated
weighted directed graphs on every run and prints checksums of the results.
Dijkstra runs five times on 12,000 nodes. Floyd-Warshall runs three times on 600
nodes.

The analysis is reproduced with:

```bash
./perf/run_perf.sh
```

`perf stat` measures each workload three times. `perf record` samples user CPU
time at 199 Hz. Instruction, branch, and cache counters returned zero in VMware,
so they were not used. The available measurements include CPU time, CPU cycles,
context switches, CPU migrations, and page faults.

#### 5.6.2 Results

| Workload | Average elapsed time | Average cycles | Main sampled functions |
| --- | ---: | ---: | --- |
| Dijkstra | 2.811 s | 7,641,854,727 | Edge-map lookup 55.05%; `reconstruct_path` 37.36% |
| Floyd-Warshall | 1.106 s | 3,188,566,971 | `FloydWarshall::run` 56.68%; `_int_malloc` 16.13%; nested-vector construction 8.29% |

Dijkstra rebuilds a full path at every visited node and saves it for
visualization. Path rebuilding and repeated edge lookup therefore take most of
the measured CPU time. Saving less data for each step would reduce this cost.

Floyd-Warshall stores complete paths in a three-level vector. This creates many
small vectors and memory allocations. `std::make_pair(distances, paths)` also
copies both result matrices. The function can return the existing matrices
without copying them. Storing only the next edge for each node pair would use
less memory.

Complete counter output, sampling reports, and workload details are stored in
`perf/results/`. The findings apply to the benchmark inputs and do not measure
GUI responsiveness.

## 6. Conclusions

The tested core logic passed all unit tests with high source coverage. Memcheck
found no memory errors in the same code paths, and Cppcheck found no confirmed
functional defect.

Lizard found four functions that are clear refactoring candidates.
clang-format also showed that formatting is inconsistent, mainly in larger GUI
files. These findings affect maintenance rather than program correctness.

perf found repeated path reconstruction in Dijkstra and unnecessary result
copying in Floyd-Warshall. Automated GUI and concurrency tests remain outside
the current analysis.
