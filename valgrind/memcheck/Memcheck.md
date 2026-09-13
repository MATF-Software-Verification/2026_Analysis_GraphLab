# Valgrind Memcheck

This directory contains the results of running Valgrind Memcheck on the QtTest
unit test executable `graphlab_unit_tests`.

## Running the Analysis

```bash
cd valgrind/memcheck
./run_memcheck.sh
```

The script expects Qt 6.8.0 at
`/home/kalu/programs/qt/6.8.0/gcc_64` by default. If Qt is installed elsewhere:

```bash
QT_PREFIX=/path/to/Qt/6.x/gcc_64 ./run_memcheck.sh
```

## Script Behavior

The script:

- configures a separate Debug build of the unit tests without coverage
  instrumentation;
- builds `graphlab_unit_tests`;
- runs the executable under Valgrind Memcheck;
- stores the complete output in `memcheck-unit-tests.txt`;
- extracts a concise result into `memcheck-summary.txt`.

The analysis covers the graph model, algorithms, and serialization code executed
by the unit tests. The GUI is not covered by this Memcheck run.

## Results

- `memcheck-unit-tests.txt`: complete build, QtTest, and Memcheck output.
- `memcheck-summary.txt`: concise Memcheck result.

The latest committed result is:

```text
26 passed, 0 failed
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
possibly lost: 0 bytes in 0 blocks
ERROR SUMMARY: 0 errors from 0 contexts
```

Memcheck reports `still reachable` memory from the Qt/GLib runtime. It is not
classified as a memory leak: `definitely lost`, `indirectly lost`, and
`possibly lost` are all zero.
