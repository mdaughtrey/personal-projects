#!/usr/bin/python

import math;

class SimpleCircle():
    def __init__(self):
        self.x = 0
        self.y = 0
        self.radius = 10

    def draw(self):
        for deg in range(0,360):
            x = math.sin(math.radians(deg)) * self.radius
            y = math.cos(math.radians(deg)) * self.radius
            print "goto %u %u 0" % (x, y)

circle = SimpleCircle()
circle.draw()
