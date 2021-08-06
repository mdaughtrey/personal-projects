import ntptime

# States
(ASLEEP, IDLE, MOVING_UP, MOVING_DOWN) = range(4)

# Events
(BUTTON, UP, DOWN, GESTURE, FAR, NEAR, TIMER) = range(7)

events = [0, 0, 0, 0, 0, 0, 0]

def ignore():
    pass

def doWakeup():
    # start Wifi
    # Get time
    pass

def doGetTime():
    pass

def doMoveUp():
    pass

def doMoveDown():
    pass

def doIdle():
    pass

# Dispatch Table
dispatch=(
    (doWakeup, ignore, ignore, ignore, ignore, ignore, doGetTime),      # ASLEEP
    (ignore, doMoveUp, doMoveDown, ignore, ignore, ignore, doGetTime),  # IDLE
    (ignore, doIdle, doIdle, doIdle, doIdle, doIdle, ignore),           # MOVING_UP
    (doIdle, doIdle, doIdle, doIdle, doItle, ignore, ignore))           # MOVING_DOWN



def setlocaltime():
    for ii in range(5):
        try:
            t = ntptime.time() - 60 * 60 * 4 # hardcoded -4 hours
            log("NTP time {}".format(t))
            tm = utime.localtime(t)
            machine.RTC().datetime((tm[0], tm[1], tm[2], tm[6] + 1, tm[3], tm[4], tm[5], 0))
            return
        except:
            log("Sleep 5 and retry {}".format(ii))
            utime.sleep(5)

def main():
    if BUTTON: events[BUTTON] = 1
    if UP: events[UP] = 1
    if DOWN: events[DOWN] = 1
    if GESTURE: events[GESTURE] =  1
    if FAR: events[FAR] = 1
    if NEAR: events[NEAR] = 1
    if TIMER: events[TIMER] = 1

    for ii in len(events):
        if events[ii]:
            dispatch[state][ii]()
            events[ii] = 0



            
