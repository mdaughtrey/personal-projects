#!/usr/bin/env python3

import pdb
int0 = 58
int1 = 267
val0 = '42'
val1 = '43'

#pdb.set_trace()
vv = 0
intervals = []
for ii in range(int0 + int1):
    if vv > 1:
        vv -= 1
        intervals.append(val0)
    else:
        intervals.append(val1)
    vv += int0/int1
print(','.join(intervals))

