#!/usr/bin/env python3

import pdb
import math
import numpy as np
import matplotlib.pyplot as plt
from Tension import Tension

startdia = 180
#startdia = 62
enddia = 35
t = Tension()
(filmlength, numframes, tension) = t.feedstats(startdia, enddia)
#(turns, uptakeEndDia) = t.uptakestats(filmlength)

x = range(numframes)
plt.plot(x, tension)
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
plt.savefig('tensions.png')
