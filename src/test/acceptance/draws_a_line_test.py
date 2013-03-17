#!/usr/bin/env python3.3
import subprocess

def test_draws_a_line():
    process = subprocess.Popen("bin/semaphore", stdout=subprocess.PIPE)
    try:
        out, err = process.communicate(timeout=1)
    except TimeoutExpired:
        process.kill()
        out, err = process.communicate()

    with open("test.png", "w+b") as png:
        png.write(out)
