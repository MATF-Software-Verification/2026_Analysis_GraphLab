#!/usr/bin/env python3
import os
import shutil
import subprocess
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
BUILD_DIR = SCRIPT_DIR / "build"
RESULTS_DIR = SCRIPT_DIR / "test-results"
COVERAGE_DIR = SCRIPT_DIR / "coverage"
QT_PREFIX = os.environ.get("QT_PREFIX", "/home/kalu/programs/qt/6.8.0/gcc_64")


def command_output(command):
    try:
        result = subprocess.run(
            command,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        return result.stdout.splitlines()[0] if result.stdout else ""
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "not found"


def run(command, log_path=None, cwd=None):
    process = subprocess.Popen(
        command,
        cwd=cwd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )

    lines = []
    assert process.stdout is not None
    for line in process.stdout:
        print(line, end="")
        lines.append(line)

    return_code = process.wait()
    if log_path is not None:
        log_path.write_text("".join(lines), encoding="utf-8")

    if return_code != 0:
        raise subprocess.CalledProcessError(return_code, command)


def main():
    shutil.rmtree(BUILD_DIR, ignore_errors=True)
    shutil.rmtree(RESULTS_DIR, ignore_errors=True)
    shutil.rmtree(COVERAGE_DIR, ignore_errors=True)
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)
    COVERAGE_DIR.mkdir(parents=True, exist_ok=True)

    environment = "\n".join(
        [
            f"cmake: {command_output(['cmake', '--version'])}",
            f"c++: {command_output([os.environ.get('CXX', 'c++'), '--version'])}",
            f"ctest: {command_output(['ctest', '--version'])}",
            f"lcov: {command_output(['lcov', '--version'])}",
            f"Qt prefix: {QT_PREFIX}",
            "",
        ]
    )
    print(environment, end="")
    (RESULTS_DIR / "environment.txt").write_text(environment, encoding="utf-8")

    run(
        [
            "cmake",
            "-S",
            str(SCRIPT_DIR),
            "-B",
            str(BUILD_DIR),
            "-DCMAKE_BUILD_TYPE=Debug",
            f"-DCMAKE_PREFIX_PATH={QT_PREFIX}",
            "-DENABLE_COVERAGE=ON",
        ],
        RESULTS_DIR / "cmake-configure.txt",
    )

    run(
        [
            "cmake",
            "--build",
            str(BUILD_DIR),
            "--target",
            "graphlab_unit_tests",
            f"-j{os.cpu_count() or 1}",
        ],
        RESULTS_DIR / "build.txt",
    )

    run(
        ["cmake", "-E", "chdir", str(BUILD_DIR), "ctest", "--output-on-failure"],
        RESULTS_DIR / "ctest-output.txt",
    )

    run(
        [str(BUILD_DIR / "graphlab_unit_tests"), "-txt"],
        RESULTS_DIR / "qtest-output.txt",
    )

    gcov_tool = shutil.which("gcov-10") or shutil.which("gcov") or "gcov"
    raw_coverage = COVERAGE_DIR / "lcov.raw.info"
    filtered_coverage = COVERAGE_DIR / "lcov.info"

    run(
        [
            "lcov",
            "--capture",
            "--directory",
            str(BUILD_DIR),
            "--gcov-tool",
            gcov_tool,
            "--output-file",
            str(raw_coverage),
        ],
        COVERAGE_DIR / "lcov-capture.txt",
    )

    run(
        [
            "lcov",
            "--remove",
            str(raw_coverage),
            "/usr/*",
            f"{QT_PREFIX}/*",
            "*/_deps/*",
            "*/unit_tests/tests/*",
            "*/unit_tests/build/*",
            "--gcov-tool",
            gcov_tool,
            "--output-file",
            str(filtered_coverage),
        ],
        COVERAGE_DIR / "lcov-filter.txt",
    )

    run(
        [
            "genhtml",
            str(filtered_coverage),
            "--output-directory",
            str(COVERAGE_DIR / "html"),
        ],
        COVERAGE_DIR / "genhtml-output.txt",
    )

    print(f"Unit test results: {RESULTS_DIR}")
    print(f"Coverage report: {COVERAGE_DIR / 'html' / 'index.html'}")


if __name__ == "__main__":
    main()
