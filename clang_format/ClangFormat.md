# clang-format Style Analysis

This directory contains a non-modifying format check of the GraphLab C++ source code.

## Dependencies

- clang-format 10 or a compatible newer version
- Bash

On Ubuntu, clang-format can be installed with:

```bash
sudo apt install clang-format
```

## Running the Analysis

From the repository root, run:

```bash
./clang_format/run_clang_format.sh
```

The GraphLab revision does not contain a `.clang-format` configuration. The
source commonly uses four-space indentation and braces on separate lines for
functions and classes. The predefined Microsoft style was selected as a close,
well-known reference without creating a custom format. The script also records
a comparison with the other predefined styles.

Production `.cpp`, `.hpp`, and `.h` files under `GraphLab/src` are checked,
excluding local build output and `graph.test.cpp`.

Each file is checked with `--dry-run --Werror`; no source file is reformatted or
otherwise modified.

## Results

- `results/clang-format-report.txt` contains the diagnostic count for each
  non-conforming file.
- `results/clang-format-summary.txt` contains aggregate results.
- `results/style-comparison.txt` compares the predefined clang-format styles.

clang-format 10.0.0 checked 66 files. All 66 had at least one difference from
the Microsoft style, producing 2,603 formatting diagnostics. The largest counts
were recorded for:

| File | Diagnostics |
| --- | ---: |
| `UI/GraphEditor/grapheditor.cpp` | 254 |
| `UI/GraphEditor/Scene/drawingedge.cpp` | 182 |
| `mainwindow.cpp` | 154 |
| `graph/graph.cpp` | 128 |
| `UI/GraphEditor/Menu/Algorithm/algorthmwidget.cpp` | 114 |

A diagnostic marks a place where clang-format would make a replacement; it is
not a count of incorrect lines or functional errors. The main differences were:

| Difference | Example | Microsoft style |
| --- | --- | --- |
| Control statement braces | `graph/graph.cpp:22` places the `{` after the `for` condition. | Places the opening brace on the next line. |
| Spaces around syntax | `Synchronization/runthread.cpp:22` uses `if(canExecute){`. | Uses spaces after the keyword and before the brace. |
| Pointer and reference alignment | `graph/graph.cpp:7` uses `const title_t& title`. | Uses `const title_t &title`. |
| Comma spacing | `UI/GraphEditor/Scene/drawingedge.cpp:8` omits spaces between parameters. | Adds a space after each comma. |
| Comment spacing | `graph/graph.cpp:21` uses `//delete...`. | Adds a space after `//`. |
| Include order and long lines | Local headers are sometimes placed after library headers, and several GUI signal connections exceed 120 columns. | Sorts includes and wraps lines at 120 columns. |

Seventeen files produced no more than ten diagnostics, while most differences
were concentrated in larger GUI files. The findings show inconsistent
formatting against Microsoft formatting style. No source
file was changed.
