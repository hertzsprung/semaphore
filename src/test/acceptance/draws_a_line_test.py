#!/usr/bin/env python3.3
import subprocess
import shlex
import os

def test_draws_a_line():
    commands = b'SCREENSHOT myscreen.png\nKEY\nSCREENSHOT myscreen2.png\nSCREENSHOT myscreen3.png\n'
    args = shlex.split(os.getenv("EXECUTABLE"));
    process = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        out, err = process.communicate(input=commands, timeout=3)
    except subprocess.TimeoutExpired:
        process.kill()
        out, err = process.communicate()
        raise
    finally:
       assert process.returncode == 0
