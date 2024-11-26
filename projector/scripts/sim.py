#!/usr/bin/env python3

import pdb
import math
import numpy as np
import matplotlib.pyplot as plt

rOuter = 90 # radius outer mm
rInner = 17 # radius inner mm
dOuter = rOuter * 2
dInner = rInner * 2

thickness = 0.14 # mm

ii = 0
circums = list()
while ii < (dOuter - dInner) / (2*thickness):
    circums.append(math.pi * (dInner+ii*thickness))
    ii += 2 * thickness

plt.plot(range(len(circums)), circums)
plt.savefig("sim.png")
pdb.set_trace()

#circums = [ math.pi * (dInner+2*x*thickness)  for x in range(dOuter - dInner) / 2*thickness)]






tMax = 50 # tension outer
tMin = 0 # tension inner

tRange = tMax - tMin
tAdj = (rOuter -rInner) / tRange

print("tRange {} tAdj {}".format(tRange, tAdj))

for r in range (rInner, rOuter, 1):
    t = tMin + (tRange * np.log10(1+(r - rInner)/(rOuter - rInner) * 10))
    print("Radius {} tension {}".format(r,t))

tensions = [tMin + (tRange * np.log10(1+(r - rInner)/(rOuter - rInner) * 10)) for r in range (rInner, rOuter, 1)]

x = range(rInner, rOuter)
#x = np.linspace(0.001, 16, 2000)
#y_e = np.log(x)
#y_10 = np.log10(x)
#y_2 = np.log2(x)
#
plt.plot(x, tensions)
#scale = rOuter/tMax
#plt.plot(x, [tMin + r * (tMax/rOuter) for r in range (rInner, rOuter, 1)])
plt.plot(x, [r * (tMax/rOuter) for r in range (rInner, rOuter, 1)])

#plt.plot(x, y_10)
#plt.plot(x, y_2)
#
#plt.legend(['ln', 'log10', 'log2'], loc='lower right')
#plt.xticks(range(math.floor(min(x)), math.ceil(max(x))+1))
#plt.axhline(0, color='black', linewidth='0.5')
plt.axvline(37, color='black', linewidth='0.5')
#
plt.savefig('tensions.png')
