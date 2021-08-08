from device import apds9960, uAPDS9960
from apds9960.const import *
from apds9960 import uAPDS9960 as APDS9960
from machine import Pin, I2C
import pdb
#import ntptime
from time import sleep

bus = I2C(sda=Pin(13), scl=Pin(14))
apds = APDS9960(bus)


dirs = {
    APDS9960_DIR_NONE: "none",
    APDS9960_DIR_LEFT: "left",
    APDS9960_DIR_RIGHT: "right",
    APDS9960_DIR_UP: "up",
    APDS9960_DIR_DOWN: "down",
    APDS9960_DIR_NEAR: "near",
    APDS9960_DIR_FAR: "far",
}

# States
(ASLEEP, IDLE, MOVING_UP, MOVING_DOWN) = range(4)
sName = ('asleep', 'idle', 'moving up', 'moving down')

# Events
(BUTTON, UP, DOWN, GESTURE, FAR, NEAR, TIMER) = range(7)


def setLocalTime():
    print("setLocalTime")
    pass
#    for ii in range(5):
#        try:
#            t = ntptime.time() - 60 * 60 * 4 # hardcoded -4 hours
#            log("NTP time {}".format(t))
#            tm = utime.localtime(t)
#            machine.RTC().datetime((tm[0], tm[1], tm[2], tm[6] + 1, tm[3], tm[4], tm[5], 0))
#            return
#        except:
#            log("Sleep 5 and retry {}".format(ii))
#            utime.sleep(5)

def ignore(state):
    print("doIgnore")
    return (state, 1)

def doWakeup(state):
    print("doWakeup")
    # start Wifi
    # Get time
    setLocalTime()
    return (IDLE, 0)

def doGetTime(state):
    print("doGetTime")
    setLocalTime()
    return (IDLE, 0)

def doMoveUp(state):
    print("doMoveUp")
    return (MOVING_UP, 0)

def doMoveDown(state):
    print("doMoveDown")
    return (MOVING_DOWN, 0)

def doIdle(state):
    print("doIdle")
    # stop all motion
    return (IDLE, 0)

def getDistance(events):
    return events

def getGesture(events):
    if apds.isGestureAvailable():
        motion = apds.readGesture()
        print("Gesture={}".format(dirs.get(motion, "unknown")))
    return events

def getButton(events): # for debug, change to interrupt
    return events

# Dispatch Table
dispatch=(
    (doWakeup, ignore,   ignore,     ignore, ignore, ignore, doGetTime),  # ASLEEP
    (ignore,   doMoveUp, doMoveDown, ignore, ignore, ignore, doGetTime),  # IDLE
    (doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore),     # MOVING_UP
    (doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore))     # MOVING_DOWN
#    BUTTON    UP        DOWN        GESTURE FAR     NEAR    TIMER

def main():
    apds.setProximityIntLowThreshold(50)
    apds.enableGestureSensor()
    events = [0, 0, 0, 0, 0, 0, 0]
    (state, _) = doWakeup(0)
    while True:
        print("State is {}".format(sName[state]))

        for ii in [getDistance, getGesture, getButton]:
            events = ii(events)

        pdb.set_trace()
        for ii in [i for i, v in enumerate(events) if v]:
            (state, events[ii]) = dispatch[state][ii](state)

main()
