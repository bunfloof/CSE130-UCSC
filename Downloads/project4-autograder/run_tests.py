import subprocess
from subprocess import run
from pathlib import Path
import os
import tempfile
from textwrap import dedent
from pytest_utils.decorators import max_score, visibility, tags

# --- CONSTANTS ---

TIMEOUT = 10
parent_dir = Path(__file__).parent

# --- HELPER FUNCTIONS ---


def parse_output(output: str):
    lines = output.split("\n")
    body = lines[:-7]
    [
        get_count_cache_l,
        get_count_disk_l,
        get_hit_rate_l,
        set_count_cache_l,
        set_count_disk_l,
        set_hit_rate_l,
    ] = lines[-7:-1]
    print(lines[-7:-1])
    get_count_cache = int(get_count_cache_l.split(":")[1].strip())
    get_count_disk = int(get_count_disk_l.split(":")[1].strip())
    get_hit_rate = float(get_hit_rate_l.split(":")[1].replace("%", "").strip())
    set_count_cache = int(set_count_cache_l.split(":")[1].strip())
    set_count_disk = int(set_count_disk_l.split(":")[1].strip())
    set_hit_rate = float(set_hit_rate_l.split(":")[1].replace("%", "").strip())
    return (
        body,
        get_count_cache,
        get_count_disk,
        get_hit_rate,
        set_count_cache,
        set_count_disk,
        set_hit_rate,
    )


# --- MAKE AND BUILD ---
@visibility("visible")
@max_score(5)
def test_make():
    r = run(["make"], capture_output=True)
    assert r.returncode == 0, r.stderr.decode()
    assert Path("./client").is_file(), "client does not exist"


@visibility("visible")
@max_score(5)
def test_code_format():
    rc = os.system(
        f"clang-format --dry-run --Werror {str(parent_dir)}/*.c {str(parent_dir)}/*.h"
    )
    assert rc == 0


@visibility("visible")
@max_score(5)
def test_basic_fifo():
    with tempfile.TemporaryDirectory() as t:
        os.system(f"echo -n hey > {t}/file1.txt")
        os.system(f"echo -n this is another file > {t}/file2.txt")
        os.system(f"echo -n Another one > {t}/file3.txt")
        r = run(
            ["./client", t, "FIFO", "2"],
            input=dedent(
                """
                GET file1.txt
                GET file2.txt
                GET file3.txt
                GET file3.txt
                GET file1.txt
            """
            ),
            encoding="utf-8",
            capture_output=True,
        )
        assert r.returncode == 0
        out = str(r.stdout)
        (
            body,
            get_count_cache,
            get_count_disk,
            get_hit_rate,
            set_count_cache,
            set_count_disk,
            _,
        ) = parse_output(out)
        assert body == [
            "hey",
            "this is another file",
            "Another one",
            "Another one",
            "hey",
        ]
        assert get_count_cache == 5
        assert get_count_disk == 4
        assert get_hit_rate == 20.00
        assert set_count_cache == 0
        assert set_count_disk == 0