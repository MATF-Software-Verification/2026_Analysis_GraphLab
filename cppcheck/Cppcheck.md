# Cppcheck Static Analysis

This directory contains the Cppcheck static analysis of the GraphLab production
C++ source.

## Dependencies

- Cppcheck 1.90 or a compatible newer version
- Bash

On Ubuntu, Cppcheck can be installed with:

```bash
sudo apt install cppcheck
```

## Running the Analysis

From the repository root, run:

```bash
./cppcheck/run_cppcheck.sh
```

The executable can be overridden when necessary:

```bash
CPPCHECK_BIN=/path/to/cppcheck ./cppcheck/run_cppcheck.sh
```

The script analyzes production `.cpp`, `.hpp`, and `.h` files under
`GraphLab/src`. Local build output and the original `graph.test.cpp` test file
are excluded. Error checks are enabled by default; the script additionally
enables the `warning` and `portability` categories. Performance and style checks
are deliberately excluded because they are covered by separate analyses. The
`--inconclusive` option also enables findings for which Cppcheck cannot
determine with certainty whether they represent a defect.

## Results

- `results/cppcheck-report.txt` contains the complete output.
- `results/cppcheck-summary.txt` contains the exit status and reported findings.

Cppcheck 1.90 reported two warnings. The first is:

```text
GraphLab/src/UI/GraphEditor/Scene/drawingscene.cpp:83:17:
warning: Assignment of function parameter has no effect outside the function.
[uselessAssignmentPtrArg]
```

The assignment `endNode = nullptr` changes only the local copy of the pointer at
the end of `DrawingScene::onNodeClickedForAddEdge`. Removing it would not change
the caller's pointer or the behavior of the function. The finding is therefore
confirmed as redundant code, but it does not represent a functional defect.

The second warning is marked as inconclusive:

```text
GraphLab/src/UI/GraphEditor/Scene/drawingedge.cpp:40:14:
warning: Member variable 'DrawingEdge::mutex' is not initialized in the
constructor. [uninitMemberVar]
```

The member is declared as a `QMutex` object rather than a pointer. Its default
constructor is invoked automatically when a `DrawingEdge` is constructed, even
though the member is not explicitly listed in the initializer list. The member
is also unused in the analyzed revision. This finding is classified as a false
positive caused by the incomplete analysis of the Qt type.

No error-severity or portability findings were reported, and neither warning
exposes a confirmed functional defect.
