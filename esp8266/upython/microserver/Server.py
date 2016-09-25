import network
import socket	
import machine
from machine import Timer
import gc
import uos
import time
import neopixel
import math

class Object(object):
    pass
    
def doAction(action, servo):
    #print("doAction %s" % action)
    if 'fight' == action:
        pos = ord(uos.urandom(1))
        pos %= (115 - 40)
        servo.duty(pos + 40)
        return
    # stop, manual = do nothing

def doLedMode(led):
    if 'heartbeat' == led.mode:
        if False == hasattr(led, 'hbCount'):
            led.hbCount = 0
        led.np[0] = (int(math.sin(led.hbCount) * 255), 0, 0)
        led.hbCount += 1

def valueOf(request, key):
    vStart = request.find(key + '=')
    vEnd = request.find(' ', vStart)
    value = request[vStart + len(key) + 1:vEnd]
    return value

def handleRGB(value, led):
    np = led.np
    red, green, blue = [int(xx) for xx in value.split(',')]
    print('R %u G %u B %u' % (red, green, blue))
    np[0] = (red, green, blue)
    np.write()

def go():
    action=''
    servo = machine.PWM(machine.Pin(12), freq = 50)
    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid='upython')
    ap.config(authmode=0)
    led = Object()
    led.np = neopixel.NeoPixel(machine.Pin(14), 1)
    led.np[0] = (0, 0, 0)
    led.np.write()
    led.mode = ''

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', 80))
    actionTimer = Timer(0)
    actionTimer.init(period = 500, mode = Timer.PERIODIC, callback = lambda xx: doAction(action, servo))
    ledTimer = Timer(1)
    ledTimer.init(period = 50, mode = Timer.PERIODIC, callback = lambda xx: doLedMode(led))
    s.listen(0)	# just queue up some requests
    while True:
        time.sleep(.5)
        conn, addr = s.accept()
        request = conn.recv(1024)
        
        conn.sendall('HTTP/1.1 200 OK\nConnection: close\nServer: Tentacle\nContent-Type: text/html\n\n')

        request = str(request)
        if 'servo=' in request:
            action = 'manual'
            servo.duty(int(valueOf(request, 'servo')))
        elif 'rgb=' in request:
            handleRGB(valueOf(request, 'rgb'), led)
        elif 'action=' in request:
            todo = valueOf(request, 'action')
            if 'fight' == todo: 
                action = 'fight'
                led.mode = 'fight'
            elif 'reset' == todo: machine.reset()
            elif 'stop' == todo: action = 'stop'
            elif 'ledoff' == todo: 
                led.mode = ''
                handleRGB('0,0,0', led)
            elif 'heartbeat' == todo: led.mode = 'heartbeat'
        else:
            with open('pg.htm', 'r') as html:
                conn.sendall(html.read())
                conn.sendall('\n')

        conn.close()
