import esp
import utime
import network
import ntptime
import machine

servopos = [43, 37, 31, 25, 19, 11, 49]

#sta_if = network.WLAN(network.STA_IF);
servo = machine.PWM(machine.Pin(14))
servo.freq(20)

def setlocaltime():
    t = ntptime.time() - 60 * 60 * 4 # hardcoded -4 hours
    print("NTP time {}".format(t))
    import machine
    import utime
    tm = utime.localtime(t)
    machine.RTC().datetime((tm[0], tm[1], tm[2], tm[6] + 1, tm[3], tm[4], tm[5], 0))

def wakeup():
    print("Waking up")
    sta_if = network.WLAN(network.STA_IF);
    sta_if.active(True)
    sta_if.scan()                             # Scan for available access points
    sta_if.connect("Zooma223", "N0stromo") # Connect to an AP
    servo.duty(servopos[0])
    while False == sta_if.isconnected():
        print("Waiting to connect")
        utime.sleep(1)
    servo.duty(servopos[1])
    print("Connected {}".format(sta_if.isconnected()))

    setlocaltime()
    lt = utime.localtime()
    dow = lt[6]
    print ("DOW {}".format(dow))
    sta_if.active(False)
    #sta_if.disconnect()
    sta_if.active(False)
#    return servopos[dow]
    servo.duty(servopos[dow])
    print("Servo duty {}".format(servo.duty()))
    sleepfor = 86500 - (lt[3] * 3600) - (lt[4] * 60) - lt[5]
    print("Sleeping for {} secs".format(sleepfor))
    return sleepfor


def sweep():
    for ii in servopos:
        utime.sleep(1)
        servo.duty(ii)

def main():
    while True:
        utime.sleep(wakeup())
        #wakeup()
        #rtc = machine.RTC()
        #rtc.irq(trigger = rtc.ALARM0, wake=machine.DEEPSLEEP)
        #utime.sleep(10)
        #rtc.alarm(rtc.ALARM0, 10000)
        #machine.deepsleep()
