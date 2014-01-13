#!/usr/bin/python

import math
import pdb

x = 0
y = 0

for deg in range(0, 360):
    x = math.sin(math.radians(deg)) + x
    y = math.cos(math.radians(deg)) + y
    print "goto %u %u 0\r" % (x, y)
