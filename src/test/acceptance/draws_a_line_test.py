#!/usr/bin/env python3.3
import subprocess
import shlex
import os

def test_draws_a_line():
    commands = b'SCREENSHOT myscreen.png\nKEY\nSCREENSHOT myscreen2.png\nSCREENSHOT myscreen3.png\n'
    cmd = " ".join([
        os.getenv("VALGRIND"),
        os.getenv("VALGRINDFLAGS"),
        "-q",
        "--error-exitcode=1",
        "--log-file=" + os.getenv("VALGRIND_LOGDIR") + "/" + __name__,
        os.getenv("EXECUTABLE")])
    args = shlex.split(cmd)
    print(cmd)
    process = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        out, err = process.communicate(input=commands, timeout=10)
    except subprocess.TimeoutExpired:
        process.kill()
        out, err = process.communicate()
        raise
    finally:
       assert process.returncode == 0
