import esp
import math
import time
import utime
import network
import ntptime
import machine
import webrepl

gamma = [0, 0, 1, 2, 4, 6, 9, 13, 16, 21, 26, 31, 37, 44, 51, 58, 66, 74, 84, 93, 103, 114, 125, 136, 148, 161, 174, 188, 202, 217, 232, 248, 264, 281, 298, 316, 334, 353, 372, 392, 412, 433, 455, 477, 499, 522, 545, 569, 594, 619, 644, 670, 697, 724, 752, 780, 808, 837, 867, 897, 928, 959, 991, 1023];

#           M   T   W   T   F   S   S
#servopos = [43, 37, 31, 25, 19, 11, 49]
#servopos = [22, 19, 16, 13, 10, 7, 25]
# 50 Hz
servopos = [105, 90, 75, 60, 45, 30, 125]

#sta_if = network.WLAN(network.STA_IF);

#smpower = machine.Pin(13, machine.Pin.OUT) # blue
#ledpower = machine.Pin(12, machine.Pin.OUT) # red
#ledPWM = machine.PWM(ledpower)
#ledPWM.freq(500)
SERVOFREQ = 50
#lastDOW = 0

def setlocaltime():
    for ii in range(5):
        try:
            t = ntptime.time() - 60 * 60 * 4 # hardcoded -4 hours
            print("NTP time {}".format(t))
            import machine
            import utime
            tm = utime.localtime(t)
            machine.RTC().datetime((tm[0], tm[1], tm[2], tm[6] + 1, tm[3], tm[4], tm[5], 0))
            return
        except:
            print("Sleep 5 and retry {}".format(ii))
            utime.sleep(5)

def init():
   # servo.freq(SERVOFREQ)
   # servo.duty(servopos[2])
    global sta_if
    sta_if = network.WLAN(network.STA_IF);
    sta_if.active(True)
    sta_if.scan()                             # Scan for available access points
    sta_if.connect("Zooma223", "N0stromo") # Connect to an AP
    sta_if.config(dhcp_hostname='qclock')
    while False == sta_if.isconnected():
        print("Waiting to connect")
        utime.sleep(1)

def getthetime():
    print("Waking up")
    global lastDOW
    #servo.duty(servopos[0])
    print("Connected {}".format(sta_if.isconnected()))
    setlocaltime()
    lt = utime.localtime()
    dow = lt[6]
    print ("DOW {}".format(dow))
    #sta_if.active(False)
    #sta_if.disconnect()
    #sta_if.active(False)
#    return servopos[dow]
    if dow != lastDOW:
        lastDOW = dow
        servo = machine.PWM(machine.Pin(14))
        servo.freq(SERVOFREQ)
        servo.duty(servopos[dow])
        print("Servo duty {}".format(servo.duty()))
        utime.sleep(2)
        servo.deinit()
#    sleepfor = 86500 - (lt[3] * 3600) - (lt[4] * 60) - lt[5]
#    print("Sleeping for {} secs".format(sleepfor))
#    return sleepfor


def sweep():
#    ledPWM.duty(0)
#    servo.freq(SERVOFREQ)
    servo = machine.PWM(machine.Pin(14))
    servo.freq(SERVOFREQ)
    for ii in servopos:
        servo.duty(ii)
        utime.sleep(1)
    servo.deinit()


#def pulse():
#    servo.duty(0)
##    servo.duty(servopos[0])
#    #ledPWM.freq(500)
#    ledpower.on()
#    for ii in gamma + gamma[::-1]:
#        ledPWM.duty(ii)
#        utime.sleep(0.01)
#    ledpower.off()
#        
def main():
    init()
    webrepl.start()
#$    sweep()
#$    utime.sleep(1)
#$#    pulse()
#$    while True:
#$        getthetime()
#$        lt = utime.localtime()
#$        togo = 86400 - (lt[3] * 3600) - (lt[4] * 60) - lt[5]
#$        print("{} seconds to go".format(togo))
#$        utime.sleep(30)
#$        #utime.sleep(togo)
