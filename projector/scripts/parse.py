#!/usr/bin/python3

import pdb
import re
import numpy as np

arr = np.empty([1,], dtype=int)
#pdb.set_trace()

for m in open('hqcap.log', 'r').readlines():
    match = re.search('isr.count: (\d+)', m.strip())
    if not match is None:
        arr = np.append(arr, int(match.group(1)))

def mapfun(x): 
    return re.search('isr.count: (\d+)', x.strip())

#a = list(map(lambda x: re.search('isr.count: (\d+)', x.strip()), open('hqcap.log', 'r').readlines()))
a = list(map(mapfun, open('hqcap.log', 'r').readlines()))
b = [int(b.group(1)) for b in a if b is not None]
deltas = np.diff(np.reshape(b,(-1,2)), axis=1)[:,0]
pdb.set_trace()
#s0 = np.sum(np.reshape(b,(-1,2)), axis=1)
#s1 = np.diff(np.reshape(s0, (-1,2)), axis=1)
print(s1)
