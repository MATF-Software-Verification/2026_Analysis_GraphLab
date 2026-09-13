# Unit Tests

This directory contains QtTest unit tests for the isolated GraphLab logic: the
graph model, graph algorithms, generic algorithm iteration logic, and `.graph`
file serialization. Tests are built from the analysis repository using source
files from `GraphLab/src`.

## Dependencies

The following dependencies are required:

- Python 3
- CMake
- a C++ compiler with C++17 support
- Qt 6 with the `Core` and `Test` modules
- `ctest`
- `lcov` and `genhtml`
- `gcov` or `gcov-10`

On Ubuntu, some of these dependencies can be installed with:

```bash
sudo apt install cmake g++ lcov
```

Qt is loaded from a local installation. The default path is
`/home/kalu/programs/qt/6.8.0/gcc_64`. A different path can be provided through
the `QT_PREFIX` environment variable.

## Running the Tests

```bash
cd unit_tests
./run_tests.py
```

If Qt is installed at a different location:

```bash
QT_PREFIX=/path/to/Qt/6.x/gcc_64 ./run_tests.py
```

## Results

The script configures the CMake project, builds the tests, executes them through
CTest and QtTest, and generates coverage using `lcov` and `genhtml`.

The latest committed result is:

```text
26 passed, 0 failed
Line coverage: 93.6%
Function coverage: 85.8%
```

Tests are divided by area:

- `tests/graph_unit_tests.cpp`: core graph operations and QVariant conversion.
- `tests/algorithm_unit_tests.cpp`: graph algorithms and algorithm iterations.
- `tests/serialization_unit_tests.cpp`: saving, loading, and missing-file
  behavior.

Generated results are stored in:

- `test-results/`: configuration, build, and test logs.
- `test-results/qtest-output.txt`: the QtTest result.
- `coverage/lcov.info`: filtered coverage data.
- `coverage/html/index.html`: the HTML coverage report.

Open the HTML coverage report with:

```bash
xdg-open coverage/html/index.html
```

Coverage applies to the isolated non-GUI logic in `GraphLab/src/graph`,
`GraphLab/src/algorithm`, and `GraphLab/src/Serialization`. GUI files are not
included in this coverage report.
