#!/usr/bin/python

import math
import pdb
import sys

DIA_MAX=2250
DIA_MIN=1250

TENSION_MAX = 10
TENSION_MIN
FRAMES = 5000

tensions = []
for frame in xrange(0, FRAMES):
    range = TENSION_START - TENSION_END
    angle = math.radians((90*frame/FRAMES)+90)
    sine = math.sin(angle)
    tension = (sine*(TENSION_START-TENSION_END))+TENSION_END
    tensions.append(int(math.floor(tension)))

for val, count in [(elem, tensions.count(elem)) for elem in sorted(set(tensions), reverse=True)]:
    print "%s %s" % (val, count)
    #sys.stdout.write("%s %s " % (val, count))
