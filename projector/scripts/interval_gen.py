#!/usr/bin/env python3

import pdb
int0 = 50
int1 = 292
val0 = '42'
val1 = '43'

#pdb.set_trace()
def run0():
    vv = 0
    intervals = []
    for ii in range(int0 + int1 + 1):
    #    print('ii {} vv {}'.format(ii,vv))
        if vv > 1:
            vv -= 1
            intervals.append(val0)
        else:
            intervals.append(val1)
            vv += int0/int1/2*1.11

    print(','.join(intervals))

def run1():
    vv = 0
    i0 = 0
    i1 = 0
#    int0 = 50
#    int1 = 150
    for ii in range(int0 + int1 + 1):
        print('ii {} vv {}'.format(ii,vv))
        if vv > 1:
            vv -= 1
            i0 += 1
            print('x')
        else:
            i1 += 1
            vv += int0/int1
            print('y')

    print('i0 {} i1 {}'.format(i0, i1))

run0()

