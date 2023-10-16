#!/bin/env python3

import math
import pdb

SAMPLES=16

def main():
    with open('sinetable.h','wb') as table:
        table.write(b'uint16_t sinetable[] = {')
        for i in range(0,360,int(360/SAMPLES)):
            value = int(32768+32768*math.sin(i*math.pi/180))
            table.write(f'{value},\n'.encode())

        table.write(b'};\n')
        table.write(f'#define NUM_SINETABLE {SAMPLES}\n'.encode())
main()
