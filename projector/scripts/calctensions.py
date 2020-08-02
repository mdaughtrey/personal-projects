#!/usr/bin/env python3

import pdb
import math
import argparse
from Tension import Tension


parser = argparse.ArgumentParser()
parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
config = parser.parse_args()


t = Tension()
(length, num, tension) = t.do(config.startdia, config.enddia)

print("Length {}m, {} Frames".format(math.floor(length/1000), num))
