import esp
import math
import time
import utime
#import network
import ntptime
import machine
import neopixel
#import webrepl

NUMLEDS = 87
#gamma = [0, 0, 1, 2, 4, 6, 9, 13, 16, 21, 26, 31, 37, 44, 51, 58, 66, 74, 84, 93, 103, 114, 125, 136, 148, 161, 174, 188, 202, 217, 232, 248, 264, 281, 298, 316, 334, 353, 372, 392, 412, 433, 455, 477, 499, 522, 545, 569, 594, 619, 644, 670, 697, 724, 752, 780, 808, 837, 867, 897, 928, 959, 991, 1023];
gamma0 = [ 0, 0, 2, 13, 48, 114, 191, 247, 247, 191, 114, 48]
#gamma0 = [ 0, 0, 0, 0, 0, 1, 2, 4, 7, 13, 21, 33, 48, 67, 89, 114, 138, 167, 191, 215, 233, 247, 252]
gamma = gamma0 + gamma0[::-1]
np = neopixel.NeoPixel(machine.Pin(2), NUMLEDS)

#           M   T   W   T   F   S   S
#servopos = [43, 37, 31, 25, 19, 11, 49]
#servopos = [22, 19, 16, 13, 10, 7, 25]
# 50 Hz
servopos = [115, 95, 80, 65, 50, 30, 135]

#sta_if = network.WLAN(network.STA_IF);

# 12 servo pwer
# 13 led power
# 14 servo control
smpower = machine.Pin(12, machine.Pin.OUT) # blue
ledpower = machine.Pin(13, machine.Pin.OUT) # blue

#ledpower = machine.Pin(12, machine.Pin.OUT) # red
#ledPWM = machine.PWM(ledpower)
#ledPWM.freq(500)
SERVOFREQ = 50
lastDOW = 0

def log(msg):
    open('log.txt', 'a').write('{}\n'.format(msg))
    print(msg)

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

#def init():
#    global sta_if
#    sta_if = network.WLAN(network.STA_IF);
#    sta_if.active(True)
#    sta_if.scan()                             # Scan for available access points
#    sta_if.connect("Zooma223", "N0stromo") # Connect to an AP
#    sta_if.config(dhcp_hostname='qclock')
#    while False == sta_if.isconnected():
#        log("Waiting to connect")
#        utime.sleep(1)
#
def getthetime():
    log("Waking up")
    global lastDOW
    #servo.duty(servopos[0])
    #log("Connected {}".format(sta_if.isconnected()))
    setlocaltime()
    lt = utime.localtime()
    dow = lt[6]
    log ("DOW {}".format(dow))
    #sta_if.active(False)
    #sta_if.disconnect()
    #sta_if.active(False)
#    return servopos[dow]
    if dow != lastDOW:
        smpower.on()
        utime.sleep(0.5)
        servo = machine.PWM(machine.Pin(14))
        servo.freq(SERVOFREQ)
        servo.duty(servopos[dow])
        log("Servo duty {}".format(servo.duty()))
        utime.sleep(2)
        servo.deinit()
        utime.sleep(0.5)
        smpower.off()
    lastDOW = dow
#    sleepfor = 86500 - (lt[3] * 3600) - (lt[4] * 60) - lt[5]
#    log("Sleeping for {} secs".format(sleepfor))
#    return sleepfor


def sweep():
#    ledPWM.duty(0)
#    servo.freq(SERVOFREQ)
    smpower.on()
    utime.sleep(0.5)
    servo = machine.PWM(machine.Pin(14))
    servo.freq(SERVOFREQ)
    for ii in servopos:
        servo.duty(ii)
        utime.sleep(1)
    servo.deinit()
    utime.sleep(0.5)
    smpower.off()

def set(val):
    servo = machine.PWM(machine.Pin(14))
    servo.freq(SERVOFREQ)
    servo.duty(val)
    smpower.on()
    utime.sleep(1.0)
    smpower.off()


def toggle(pin):
    mypin = machine.Pin(pin, machine.Pin.OUT) 
    for ii in range(100):
        mypin.on()
        utime.sleep(0.5)
        mypin.off()
        utime.sleep(0.5)

def nptest(vals):
    ledpower.on()
    utime.sleep(0.1)
    
    for ii in range(NUMLEDS):
        for jj in range(len(gamma)):
            np[(ii + jj) % NUMLEDS] = (int(gamma[jj] * vals[0]), int(gamma[jj] * vals[1]), int(gamma[jj] * vals[2]))
        np.write()
        utime.sleep(0.01)

    np.fill((0, 0, 0))
    np.write()
    utime.sleep(1.0)
    ledpower.off()

def pulse(vals):
    ledpower.on()
    utime.sleep(0.1)
    for ii in range(len(gamma)):
        np.fill((int(gamma[ii] * vals[0]), int(gamma[ii] * vals[1]), int(gamma[ii] * vals[2])))
        np.write()

    np.fill((0, 0, 0))
    np.write()
    utime.sleep(1.0)
    ledpower.off()
    

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
    sweep()
    utime.sleep(1)
    smpower.off()
    ledpower.off()
    while True:
        getthetime()
        lt = utime.localtime()
        togo = 86400 - (lt[3] * 3600) - (lt[4] * 60) - lt[5]
        log("{} seconds to go".format(togo))
        utime.sleep(togo)
#        utime.sleep(30)
