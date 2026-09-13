# Lizard Complexity Analysis

This directory contains a source-code complexity analysis of GraphLab using
Lizard 1.21.2.

## Dependencies

- Python 3.8 or newer
- Lizard 1.21.2
- Bash

Install the pinned Lizard version with:

```bash
python3 -m pip install -r lizard/requirements.txt
```

## Running the Analysis

From the repository root, run:

```bash
./lizard/run_lizard.sh
```

The script analyzes production `.cpp`, `.hpp`, and `.h` files under
`GraphLab/src`, excluding local build output and `graph.test.cpp`. It reports
functions that exceed at least one of these thresholds:

- cyclomatic complexity (CCN) greater than 10;
- non-comment lines of code (NLOC) greater than 50;
- more than 5 parameters.

The thresholds identify review candidates.

## Results

- `results/lizard-report.txt` contains metrics for every analyzed function and
  file.
- `results/lizard-summary.txt` contains the configured thresholds and all
  threshold warnings.

Lizard analyzed 66 files containing 4,583 NLOC and 259 functions. Twelve
functions exceeded at least one configured threshold. Seven exceeded the CCN
threshold; the other five were reported only because of size or parameter
count.

| Finding | Assessment | Proposed action |
| --- | --- | --- |
| `RunThread::run` (`Synchronization/runthread.cpp`) (CCN 30, NLOC 176) | **Refactoring candidate.** It runs every algorithm and also changes the UI. | Move each algorithm into its own function. |
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

Manual review identified four refactoring candidates. The other eight warnings
were accepted because they reflect algorithm structure, function size, or
parameter count rather than excessive control-flow complexity.
