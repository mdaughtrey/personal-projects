#!/usr/bin/env python3

import pdb
import numpy as np

centers = np.zeros((100,2), np.int32)

def init():
    global centers
    ar0 = np.random.randint(1, 100, 10)
    ar1 = np.random.randint(1, 100, 10)
    centers = np.column_stack((ar0, ar1))

def avg_center(center):
    global centers
    if center is not None:
        centers = np.append(centers[1:], [center], axis=0)
    weights = list(range(1, 1+len(centers)))
    totalweight = sum(weights)
#    x, y = 0, 0
#    for ii in range(len(centers)):
#        x = x + centers[ii][0] * (1+ii) / totalweight
#        y = y + centers[ii][1] * (1+ii) / totalweight

    xa = sum([ii*jj for ii,jj in zip(centers[:,0],weights)]) / totalweight
    ya = sum([ii*jj for ii,jj in zip(centers[:,1],weights)]) / totalweight
#    print("x {} y {} xa {} ya {}".format(x,y,xa,ya))
    print("xa {} ya {}".format(xa,ya))
    return (int(xa),int(ya))
#    return (int(x),int(y))

init()
avg_center(None)
avg_center([50,49])
