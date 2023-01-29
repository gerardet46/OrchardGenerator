"""
C wrapper to generate smallest CPS.

Make sure you are in the root of the python project
"""
import os
import subprocess

DIR = "C/"

def generate(n, l, o_all, make=True, condition="None", output="Seq", parallel=0): # noqa
    """
    Generator of all orchards networks using C.

    All the options are the same as compile() (see below),
    except for `make` which is True to compile or False not
    to compile (you will execute a previous version)
    """
    if make:
        compile(n, l, o_all, condition, output, parallel)

    os.chdir(DIR)
    try:
        proc = subprocess.Popen(['./generate'], stdout=subprocess.PIPE)
        while True:
            line = proc.stdout.readline()
            if line:
                yield line[:-1].decode()
            else:
                break
    finally:
        os.chdir("..")


def generate_random(n, l, q=1, make=True, condition="None", output="Seq", parallel=0): # noqa
    """
    Generator of random orchards networks using C.

    All the options are the same as generate() (see above),
    except for `q` which is the number of desired random
    generated networks (there might be repetitions)
    """
    if make:
        compile(n, l, 0, condition, output, parallel)

    os.chdir(DIR)
    try:
        proc = subprocess.Popen(['./randgen', str(q)], stdout=subprocess.PIPE)
        while True:
            line = proc.stdout.readline()
            if line:
                yield line[:-1].decode()
            else:
                break
    finally:
        os.chdir("..")


def compile(n, l, o_all, condition="None", output="Seq", parallel=0): # noqa
    """
    Compile C generator.

    Options:
    * n: number of leaves
    * l: max. length of the sequence
    * o_all: 1 to output "less or equal than l"
    * condition: "None" (all nets)
                 "SF"   (stack-free)
                 "TC"   (tree-child)
    * output: "Nothing"
              "Seq"     (output sequences)
              "Pol"     (output readable polynomial)
              "TexPol"  (output polynomial in LaTeX)
    * parallel: 1 to parallel executiion (ONLY UNIX SYSTEMS!)
    """
    options = {
        "N": n,
        "MAXLENGTH": l,
        "CONDITION": "cond" + condition,
        "OUTPUT_FUNC": "print" + output,
        "OUTPUT_ALL": int(o_all),
        "OUTPUT_TOTAL": 0,
        "COUNT_SECONDS": 0,
        "PARALLEL": parallel,
        "PARALLEL_PRINT_FINISHED": 0,
    }
    command = ["make", "all", "clean"] + [f"{k}={options[k]}" for k in options]

    os.chdir(DIR)
    try:
        subprocess.check_call(command, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT) # noqa
    finally:
        os.chdir("..")
