#!/usr/bin/env python3.3
import subprocess

def test_draws_a_line():
    commands = b'SCREENSHOT myscreen.png\nSCREENSHOT myscreen2.png\n'
    process = subprocess.Popen("bin/semaphore", stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        out, err = process.communicate(input=commands, timeout=1)
    except subprocess.TimeoutExpired:
        process.kill()
        out, err = process.communicate()
        raise

    print(out)
