# GraphLab Software Verification Analysis

This repository contains a software verification analysis of the open-source
GraphLab project. It is part of the Software Verification
course at the Faculty of Mathematics, University of Belgrade.

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
- Valgrind with the Memcheck tool.

On Ubuntu, the dependencies available through the system package manager can
be installed with:

```bash
sudo apt install git python3 cmake g++ lcov valgrind
```

Qt 6 must be installed separately if it is not provided by the system. The
scripts use `/home/kalu/programs/qt/6.8.0/gcc_64` as the default Qt prefix. A
different installation can be selected with:

```bash
export QT_PREFIX=/path/to/Qt/6.x/gcc_64
```

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

## Tools Used

| Tool | Scope | Result | Reproduction instructions |
| --- | --- | --- | --- |
| QtTest unit tests with lcov coverage | Graph model, graph algorithms, algorithm iteration logic, and serialization | 26 QtTest checks passed; 93.6% line and 85.8% function coverage | [`unit_tests/RunningTests.md`](unit_tests/RunningTests.md) |
| Valgrind Memcheck | The core logic covered by the unit test suite | No Memcheck errors and no lost memory | [`valgrind/memcheck/Memcheck.md`](valgrind/memcheck/Memcheck.md) |




## Current Conclusions

- All 24 explicitly implemented unit test cases pass. QtTest reports 26 passed
  checks because it also reports suite initialization and cleanup.
- The tested non-GUI code reaches 93.6% line coverage and 85.8% function
  coverage.
- Memcheck found no invalid memory accesses, uses of uninitialized values, or
  lost memory in the paths exercised by the unit tests.
- The current analyses do not cover GUI interactions, thread behavior, or all
  possible user-controlled `.graph` inputs.

Detailed methodology, results, limitations, and conclusions are recorded in
[`ProjectAnalysisReport.md`](ProjectAnalysisReport.md).

## Author

- **Name:** Luka Matic
- **Index:** 1014/2025
