#!/usr/bin/env python3

import pdb
import math
import numpy as np
import itertools
import matplotlib.pyplot as plt
import argparse


largestDia = 180 # mm
uptakeD0 = 65 # mm
thickness = 0.14 # mm
frameSpace8mm = 3.3 # mm
frameSpaceS8 = 4.01 # mm

maxT = 50
minT = 5
maxD = 180

parser = argparse.ArgumentParser()
parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
config = parser.parse_args()

class Tension:
    TENSION_MAX = 50
    TENSION_MIN = 5
    DIA_MAX = 180
    DIA0_UPTAKE = 65
    FILM_THICKNESS = 0.14
    FRAMESPACE_8MM = 3.3
    FRAMESPACE_S8 = 4.01

    def do(self, startdia, enddia):
        (feedTurns, filmLength) = self.calcLength(enddia, startdia, Tension.FILM_THICKNESS)
        numFrames = math.floor(filmLength / Tension.FRAMESPACE_8MM)
        feedC = self.calcCircums(enddia, math.floor(feedTurns), Tension.FILM_THICKNESS)
        feedD = self.calcDia(enddia, math.floor(feedTurns), Tension.FILM_THICKNESS)
        tPerTurn = (Tension.TENSION_MAX - Tension.TENSION_MIN) / feedTurns * startdia / Tension.DIA_MAX
        tensionPerC = [math.floor(Tension.TENSION_MIN + tPerTurn * t) for t in range(math.floor(feedTurns))][::-1]
        framesPerC = [math.floor(x/Tension.FRAMESPACE_8MM) for x in feedC][::-1]
        return [tensionPerC[self.getindex(frame, framesPerC)] for frame in range(numFrames)]
#        for frame in range(numFrames + 100):
#            index = self.getindex(frame, framesPerC)
#            print("Frame {} C Index {} Tension {}".format(frame, index, tensionPerC[index]))


    # Given an ID d0 and an OD d1 and a thickness, calculate the length
    def calcLength(self, d0, d1, thick):
        turns = (d1 - d0) / (2 * thick)
        length = math.pi * turns * (d0 + thick * (turns -1 ))
        return (turns, length)

    # Number of concentric circles
    def calcturns(self, d0, length, thick):
        a = (4 * thick * length) / math.pi
        b = math.pow(d0 - thick, 2) + a
        c = thick - d0 + math.sqrt(b)
        return c / (2 * thick)

    # Circumference of each concentric circle
    def calcCircums(self, d0, turns, thickness):
        return list([math.floor(math.pi * (d0 + x * 2 * thickness)) for x in range(turns)])

    # Diamater of each concentric circle
    def calcDia(self, d0, turns, thickness):
        return list([math.floor(d0 + x * 2 * thickness) for x in range(turns)])

    # Index of framesPerC of framenum
    def getindex(self, framenum, framesPerC):
        for ii in range(1, len(framesPerC)):
            if sum(framesPerC[:ii]) > framenum:
                return ii - 1
        return len(framesPerC)-1

t = Tension()
tension = t.do(config.startdia, config.enddia)

#(feedTurns, filmLength) = calcLength(config.enddia, config.startdia, thickness)
#
#numFrames = math.floor(filmLength / frameSpace8mm)
#feedC = calcCircums(config.enddia, math.floor(feedTurns), thickness)
#feedD = calcDia(config.enddia, math.floor(feedTurns), thickness)
#
#print("Turns {} Length {} m {} feet frames {} ".format(math.floor(feedTurns),
#    math.floor(filmLength / 1000),
#    math.floor(filmLength / 304.8),
#    math.floor(numFrames) ))
#
#tPerTurn = (maxT - minT) / feedTurns * config.startdia / largestDia
#tensionPerC = [math.floor(minT + tPerTurn * t) for t in range(math.floor(feedTurns))][::-1]
#print("tension array {}".format(tensionPerC))
#framesPerC = [math.floor(x/frameSpace8mm) for x in feedC][::-1]
#
#
## Index of framesPerC of framenum
#def getindex(framenum, framesPerC):
#    for ii in range(1, len(framesPerC)):
#        if sum(framesPerC[:ii]) > framenum:
#            return ii - 1
#    return len(framesPerC)-1
#
#for frame in range(numFrames + 100):
#    index = getindex(frame, framesPerC)
#    print("Frame {} C Index {} Tension {}".format(frame, index, tensionPerC[index]))


#print("Calculated turns {}".format(math.floor(calcturns(feedD0, filmLength, thickness))))
#
#uptakeD0 = 65 # mm
#uptakeTurns = math.floor(calcturns(uptakeD0, filmLength, thickness))
#print("Uptake turns {}".format(uptakeTurns))
#print("Uptake D1 {}".format(uptakeD0 + thickness * uptakeTurns * 2))
#uptakeD = calcDia(uptakeD0, math.floor(uptakeTurns), thickness)
#
##steps = [k * len([h for h in g]) for (k, g) in itertools.groupby(uptakeC)]
#
#pdb.set_trace()


#
#turns2 = (thickness - update) 
#
#/ (2 * thickness)
