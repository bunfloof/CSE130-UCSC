import subprocess
from subprocess import run
from pathlib import Path
import os
import tempfile
from textwrap import dedent
from pytest_utils.decorators import max_score, visibility, tags
import uuid

# --- CONSTANTS ---

TIMEOUT = 10
parent_dir = Path(__file__).parent

# --- HELPER FUNCTIONS ---
def get_id() -> str:
    return str(uuid.uuid4())[:10]

# --- MAKE AND BUILD ---
@visibility("visible")
@max_score(5)
def test_make():
    r = run(["make"], capture_output=True)
    assert r.returncode == 0, r.stderr.decode()
    assert Path("./container").is_file(), "container does not exist"


@visibility("visible")
@max_score(5)
def test_code_format():
    rc = os.system(
        f"clang-format --dry-run --Werror {str(parent_dir)}/*.c {str(parent_dir)}/*.h"
    )
    assert rc == 0
