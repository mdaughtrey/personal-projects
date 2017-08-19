#!/usr/bin/python

import math
import pdb
import sys

TENSION_START = 45
TENSION_END = 20
FRAMES = 30000

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
