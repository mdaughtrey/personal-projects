#!/bin/env python3

import pdb
import subprocess

def main():
    pdb.set_trace()
    sp = subprocess.Popen(['ls'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    print(sp.stdout.read())
    sp.stdin.write(b'pwd\n')
    print(sp.stdout.read())
    sp.terminate()


main()
