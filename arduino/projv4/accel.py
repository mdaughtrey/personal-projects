#!/usr/bin/env python3

import pdb

def genSlope(points):
    x = [(points - ii)/points for ii in range(0, points)]
#    x.extend(x[::-1])
    return x

def nextInterval(totalSteps, currentStep, minInterval, maxInterval, rampUpSteps, rampDownSteps):
    intRange = maxInterval - minInterval
    if currentStep < rampUpSteps:
        rampUp = genSlope(rampUpSteps)
        return minInterval + int(intRange * rampUp[currentStep])
    if currentStep < (totalSteps - rampDownSteps):
        return minInterval

#    pdb.set_trace()
    rampDown = genSlope(rampDownSteps)[::-1]
    return minInterval + int(intRange * rampDown[currentStep-(totalSteps-rampDownSteps)])


def main():
    totalSteps = 60
    minInterval = 1 # ms
    maxInterval = 10 # ms 
    rampUpSteps = 20
    rampDownSteps = 10

    for step in range(totalSteps):
        interval = nextInterval(totalSteps, step, minInterval, maxInterval, rampUpSteps, rampDownSteps)
        print(f'step {step} interval {interval}')


main()
