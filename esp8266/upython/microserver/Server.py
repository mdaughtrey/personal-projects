import network
import socket	
import machine
from machine import Timer
import gc
import uos
import time
#import neopixel
import math

class Object(object):
    pass
    
gammaTable = (0, 1, 2, 6, 12, 20, 31, 44, 60, 79, 100, 125, 153, 183, 218, 255)

def doServo(servo):
    #print("doServo %s" % action)
    if 'fight' == servo.mode:
        if 40 == servo.lastPos:
            servo.lastPos = 115
        else:
            servo.lastPos = 40 
        servo.motor.duty(servo.lastPos)

#        pos = ord(uos.urandom(1))
#        pos %= (115 - 40)
#        servo.motor.duty(pos + 40)
        return
    # stop, manual = do nothing

def doLedMode(led):
    if 'heartbeat' == led.mode:
        duty = int(math.sin(math.radians(led.count)) * 15)
        if duty > 0:
            led.pwm.duty(1023 - 4 * gammaTable[duty])
        led.count += 10
        led.count %= 360
    if 'fight' == led.mode:
        if 0 == led.count:
            led.count = 1023
        else:
            led.count = 0
        led.pwm.duty(led.count)

def valueOf(request, key):
    vStart = request.find(key + '=')
    vEnd = request.find(' ', vStart)
    value = request[vStart + len(key) + 1:vEnd]
    print('%s -> %s' % (key, value))
    return value

def handlePWM(value, led):
    led.pwm.duty(value)

def handleRGB(value, led):
    pass
#    np = led.np
#    red, green, blue = [int(xx) for xx in value.split(',')]
#    print('R %u G %u B %u' % (red, green, blue))
#    np[0] = (red, green, blue)
#    np.write()

def go():
    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid='upython')
    ap.config(authmode=0)
    led = Object()
#    led.np = neopixel.NeoPixel(machine.Pin(14), 1)
#    led.np[0] = (0, 0, 0)
#    led.np.write()
    led.mode = ''
    led.pwm = machine.PWM(machine.Pin(14), freq = 100, duty = 1023)
    led.count = 0

    servo = Object()
    servo.motor = machine.PWM(machine.Pin(12), freq = 50)
    servo.mode = ''
    servo.lastPos = 40
    servo.motor.duty(servo.lastPos)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', 80))
    servoTimer = Timer(0)
    servoTimer.init(period = 1000, mode = Timer.PERIODIC, callback = lambda xx: doServo(servo))
    ledTimer = Timer(1)
    ledTimer.init(period = 50, mode = Timer.PERIODIC, callback = lambda xx: doLedMode(led))
    s.listen(0)	# just queue up some requests
    while True:
        time.sleep(.5)
        conn, addr = s.accept()
        request = conn.recv(1024)
        
        conn.sendall('HTTP/1.1 200 OK\nConnection: close\nServer: Tentacle\nContent-Type: text/html\n\n')

        request = str(request)
        print(request)
        if 'servo=' in request:
            servo.mode = 'manual'
            servo.motor.duty(int(valueOf(request, 'servo')))
        elif 'rgb=' in request:
            handleRGB(valueOf(request, 'rgb'), led)
        elif 'pwm=' in request:
            handlePWM(int(valueOf(request, 'pwm')), led)
        elif 'action=' in request:
            todo = valueOf(request, 'action')
            if 'fight' == todo: 
                servo.mode= 'fight'
                led.mode = 'fight'
            elif 'reset' == todo: machine.reset()
            elif 'stop' == todo: servo.mode = 'stop'
            elif 'ledoff' == todo: 
                led.mode = ''
                handleRGB('0,0,0', led)
                handlePWM(1023, led)
            elif 'heartbeat' == todo: led.mode = 'heartbeat'
        else:
            with open('pg.htm', 'r') as html:
                conn.sendall(html.read())
                conn.sendall('\n')

        conn.close()
