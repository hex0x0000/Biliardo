#!/usr/bin/env python3

import sys
import os
from shutil import which

def compile() -> int:
    if which("cmake") and which("ninja"):
        proj_dir = os.getcwd()
        if not os.path.isdir("build"):
            os.makedirs("build")
        os.chdir("build")
        print("--> Setting up...")
        os.system(f"cmake \"{proj_dir}\" -G Ninja")
        print("--> Compiling...")
        cpus = os.cpu_count()
        os.system(f"ninja -j{cpus}")
    else:
        print("cmake not found in PATH. Exiting")
        return -1
    return 0

if __name__ == "__main__":
    sys.exit(compile())

