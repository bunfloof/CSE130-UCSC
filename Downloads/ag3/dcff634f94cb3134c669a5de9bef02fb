import subprocess
from subprocess import run
from pathlib import Path
import os
from pytest_utils.decorators import max_score, visibility, tags

# --- CONSTANTS ---

TIMEOUT = 10
parent_dir = Path(__file__).parent

# --- HELPER FUNCTIONS

CC = ["clang"]
CFLAGS = [
    "-Wall",
    "-Wextra",
    "-Werror",
    "-std=c11",
    "-pedantic",
    "-Wno-unused-parameter",
]


def build_program(program: str, out: str) -> subprocess.CompletedProcess[bytes]:
    """build_program builds a program"""
    r = run(
        CC + CFLAGS + [program, "dining.o", "utils.o", "-o", out], capture_output=True
    )
    return r


def run_and_validate(
    executable: str, capacity: int
) -> subprocess.CompletedProcess[bytes]:
    r = run(
        ["bash", "-c", f"./{executable} | ./validator.py {capacity}"],
        capture_output=True,
        timeout=TIMEOUT,
    )
    return r


# --- MAKE AND BUILD ---


@visibility("visible")
@max_score(5)
def test_make():
    r = run(["make"], capture_output=True)
    assert r.returncode == 0, r.stderr.decode()
    assert Path("./example1").is_file(), "example1 does not exist"
    assert Path("./example2").is_file(), "example2 does not exist"


@visibility("visible")
@max_score(5)
def test_code_format():
    rc = os.system(
        f"clang-format --dry-run --Werror {str(parent_dir)}/*.c {str(parent_dir)}/*.h"
    )
    assert rc == 0


@visibility("visible")
@max_score(5)
def test_example1():
    r = run_and_validate("./example1", 3)
    assert r.returncode == 0, r.stderr.decode()


@visibility("visible")
@max_score(5)
def test_example2():
    r = run_and_validate("./example2", 3)
    assert r.returncode == 0, r.stderr.decode()


@visibility("after_published")
@max_score(5)
def test_example3():
    r = build_program("./example3.c", "example3")
    assert r.returncode == 0, r.stderr.decode()

    r = run_and_validate("./example3", 3)
    assert r.returncode == 0, r.stderr.decode()
