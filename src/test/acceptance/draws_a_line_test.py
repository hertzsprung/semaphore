#!/usr/bin/env python3.3
import subprocess

def test_draws_a_line():
    commands = b'SCREENSHOT myscreen.png\nKEY\nSCREENSHOT myscreen2.png\nSCREENSHOT myscreen3.png\n'
    process = subprocess.Popen("./semaphore", stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        out, err = process.communicate(input=commands, timeout=1)
    except subprocess.TimeoutExpired:
        process.kill()
        out, err = process.communicate()
        raise
    finally:
        print(out)
