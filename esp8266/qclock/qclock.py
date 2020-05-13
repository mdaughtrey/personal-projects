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
    while False == sta_if.isconnected():
        print("Waiting to connect")
        utime.sleep(1)
    print("Connected {}".format(sta_if.isconnected()))

    setlocaltime()
    dow = utime.localtime()[6]
    print ("DOW {}".format(dow))
    sta_if.active(False)
    #sta_if.disconnect()
    servo.duty(servopos[dow])
    print("Servo duty {}".format(servo.duty()))


def sweep():
    for ii in servopos:
        utime.sleep(1.0)
        servo.duty(ii)
    for ii in servopos[::-1]:
        utime.sleep(1.0)
        servo.duty(ii)

def main():
    sweep()
    while True:
        wakeup()
        print("Sleeping for 7200 secs")
        machine.deepsleep(7200000)
        #utime.sleep(20)



