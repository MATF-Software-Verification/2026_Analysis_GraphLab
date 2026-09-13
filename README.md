# GraphLab Software Verification Analysis

This repository contains a software verification analysis of the open-source
GraphLab project. It is part of the Software Verification
course at the Faculty of Mathematics, University of Belgrade.

## Analyzed Project

[GraphLab](https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2024-2025/GraphLab)
is a Qt desktop application for creating and visualizing graphs and for running
graph algorithms step by step. It supports directed and undirected,
weighted and unweighted graphs, as well as multigraphs.

|  |  |
| --- | --- |
| Repository | <https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2024-2025/GraphLab> |
| Analyzed branch | `main` |
| Analyzed commit | `ad54f25c8f771458062385bcb2022c168b706aac` |
| Local reference | Git submodule in `GraphLab/` |
|  |  |

The analyzed source is referenced as a Git submodule. Clone this repository
with its submodule by running:

```bash
git clone --recurse-submodules git@github.com:MATF-Software-Verification/2026_Analysis_GraphLab.git
```

If the repository has already been cloned without the submodule, initialize it
with:

```bash
git submodule update --init --recursive
```

## Requirements for Reproduction

Reproducing all committed analyses requires a Linux environment with:

- Git with submodule support;
- Python 3;
- Bash and standard command-line utilities (`awk`, `head`, `nproc`, and `tee`);
- CMake 3.16 or newer, including CTest;
- a C++ compiler with C++17 support;
- Qt 6 with the `Core` and `Test` modules;
- lcov, including genhtml;
- gcov matching the GCC version used to build the tests;
- Valgrind with the Memcheck tool;
- Cppcheck;
- Python package Lizard 1.21.2;
- clang-format 10 or a compatible newer version;
- Linux perf compatible with the running kernel.

On Ubuntu, the dependencies can be installed with:

```bash
sudo apt install git python3 python3-pip cmake g++ lcov valgrind cppcheck clang-format linux-tools-common linux-tools-$(uname -r)
python3 -m pip install -r lizard/requirements.txt
```

Qt 6 must be installed separately if it is not provided by the system. The
scripts use `/home/<user>/programs/qt/6.8.0/gcc_64` as the default Qt prefix. A
different installation can be selected with:

```bash
export QT_PREFIX=/path/to/Qt/6.x/gcc_64
```

## Tools Used

| Tool | Scope | Result | Reproduction instructions |
| --- | --- | --- | --- |
| QtTest unit tests with lcov coverage | Graph model, graph algorithms, algorithm iteration logic, and serialization | 26 QtTest checks passed; 93.6% line and 85.8% function coverage | [`unit_tests/RunningTests.md`](unit_tests/RunningTests.md) |
| Valgrind Memcheck | The core logic covered by the unit test suite | No Memcheck errors and no lost memory | [`valgrind/memcheck/Memcheck.md`](valgrind/memcheck/Memcheck.md) |
| Cppcheck | Production C++ source | One confirmed redundant assignment and one inconclusive false positive; no error-severity or portability findings | [`cppcheck/Cppcheck.md`](cppcheck/Cppcheck.md) |
| Lizard | Structural complexity of C++ source | 12 threshold warnings; manual review found 4 clear refactoring candidates | [`lizard/Lizard.md`](lizard/Lizard.md) |
| clang-format | Non-modifying Microsoft-style check of production C++ source | 2,603 diagnostics across 66 files, concentrated in larger GUI files | [`clang_format/ClangFormat.md`](clang_format/ClangFormat.md) |
| Linux perf | Dijkstra and Floyd-Warshall deterministic workloads | Repeated path reconstruction and nested-vector allocation/copying were the main avoidable costs | [`perf/Perf.md`](perf/Perf.md) |

## Conclusions

- Unit tests passed with high coverage of the core non-GUI code.
- Memcheck found no errors in the tested paths.
- Cppcheck found one redundant assignment but no confirmed functional defect.
- Lizard found four refactoring candidates.
- clang-format showed inconsistent formatting, especially in larger GUI files.
- perf found repeated path reconstruction in Dijkstra and unnecessary result copying in Floyd-Warshall.
- Automated GUI and concurrency tests were not included.

Detailed methodology, results, limitations, and conclusions are recorded in
[`ProjectAnalysisReport.md`](ProjectAnalysisReport.md).

## Author

- Luka Matic, 1014/2025
