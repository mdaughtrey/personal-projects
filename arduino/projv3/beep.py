#!/usr/bin/python

import math
import pyaudio
import serial
import sys
import time
import pdb

HiTone = 1000
LoTone = 440
lastTone = LoTone

#See http://en.wikipedia.org/wiki/Bit_rate#Audio
BITRATE = 16000 #number of frames per second/frameset.      

LENGTH = 0.1 #seconds to play sound
NUMBEROFFRAMES = int(BITRATE * LENGTH)
RESTFRAMES = NUMBEROFFRAMES % BITRATE

PyAudio = pyaudio.PyAudio
p = PyAudio()
stream = p.open(format = p.get_format_from_width(1), 
                channels = 1, 
                rate = BITRATE, 
                output = True)


USBDEV='/dev/tty.usbserial-A601KW2O'

def beep(numbers):
    global lastTone
#    sys.stdout.write(numbers)
    numarray = [int(ii) for ii in numbers.split()]
    if 0 == len(numarray):
        return
    wavedata = ''    
    sys.stdout.write(numbers)
    return
    for num in numarray:
        if num < 512 and lastTone == LoTone:
            continue
        if num > 512 and lastTone == HiTone:
            continue
        sys.stdout.write("%d" % lastTone)
#        for xx in xrange(160):
#            wavedata = wavedata+chr(int(math.sin(xx/((BITRATE/lastTone)/math.pi))*127+128))    
#        stream.write(wavedata)
        if lastTone == HiTone:
            lastTone = LoTone
        else:
            lastTone = HiTone
        return


serdev = serial.Serial(USBDEV, 57600, timeout=1.0)
time.sleep(5)

serdev.write('50m')
serdev.write('-87s')
serdev.write('e')
serdev.write('D')

while True:
    waiting = serdev.inWaiting()
    if 0 == waiting:
        continue
    data = (serdev.read(waiting))
    sys.stdout.write(data)
    try:
        beep(data.split('***')[-1])
    except:
        pass

stream.stop_stream()
stream.close()
p.terminate()
