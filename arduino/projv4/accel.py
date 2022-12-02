#!/usr/bin/env python3

import pdb
import sys

def genSlope(points):
    x = [(points - ii)/points for ii in range(0, points)]
#    x.extend(x[::-1])
    return x

def nextInterval0(totalSteps, currentStep, minInterval, maxInterval, rampUpSteps, rampDownSteps):
    intRange = maxInterval - minInterval
    if currentStep < rampUpSteps:
        rampUp = genSlope(rampUpSteps)
        return minInterval + int(intRange * rampUp[currentStep])
    if currentStep < (totalSteps - rampDownSteps):
        return minInterval
    rampDown = genSlope(rampDownSteps)[::-1]
    return minInterval + int(intRange * rampDown[currentStep-(totalSteps-rampDownSteps)])

def nextInterval1(totalSteps, currentStep, minInterval, maxInterval, rampUpSteps, rampDownSteps):
    intRange = maxInterval - minInterval
    if currentStep < rampUpSteps:
        x = (rampUpSteps - currentStep)/rampUpSteps
        return minInterval + int(intRange * x)
    if currentStep < (totalSteps - rampDownSteps):
        return minInterval

    x = (rampDownSteps - (totalSteps - currentStep))/rampDownSteps
    return minInterval + int(intRange * x)


def main():
    totalSteps = int(sys.argv[1])
    minInterval = 1 # ms
    maxInterval = 4 # ms 
    rampUpSteps = 20
    rampDownSteps = 10

    for step in range(totalSteps):
        interval0 = nextInterval0(totalSteps, step, minInterval, maxInterval, rampUpSteps, rampDownSteps)
        interval1 = nextInterval1(totalSteps, step, minInterval, maxInterval, rampUpSteps, rampDownSteps)
        print(f'step {step} interval0 {interval0} interval1 {interval1}')


main()
