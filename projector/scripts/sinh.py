#!/usr/bin/env python3

import pdb
import math
import numpy as np
import matplotlib.pyplot as plt
from Tension import Tension

startdia = 180
enddia = 65
#t = Tension()
#(filmlength, numframes, tension) = t.do(startdia, enddia)

coreR = 62/2;
#x = range(len(tension))
#x = radius
x = range(-300, 300)
y = [np.tanh(x/100) for x in x]
#plt.plot(x, tension)
plt.plot(x, y)
#scale = rOuter/tMax
#plt.plot(x, [tMin + r * (tMax/rOuter) for r in range (rInner, rOuter, 1)])
#plt.plot(x, [r * (tMax/rOuter) for r in range (rInner, rOuter, 1)])

#plt.plot(x, y_10)
#plt.plot(x, y_2)
#
#plt.legend(['ln', 'log10', 'log2'], loc='lower right')
#plt.xticks(range(math.floor(min(x)), math.ceil(max(x))+1))
#plt.axhline(0, color='black', linewidth='0.5')
#plt.axvline(37, color='black', linewidth='0.5')
#
plt.savefig('sinh.png')
