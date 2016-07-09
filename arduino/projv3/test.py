#!/usr/bin/python

import serial
import time
import sys
import pdb

#ptStartValue = 179
ptStartValue = 160
ptEndValue = 244
ptArray = range(ptStartValue, ptEndValue)
chunkSize = int(len(ptArray)/10)

if len(sys.argv) != 3:
    print "Usage: cycle.py framestart frameend"
    sys.exit(1)

USBDEV='/dev/tty.usbserial-A601KW2O'

serdev = serial.Serial(USBDEV, 57600, timeout=1.0)
time.sleep(2)
(frameStart, frameEnd) = [int(xx) for xx in sys.argv[1:3]]
frameCount = frameStart

def swrite(data):
#    print "Serial: %s" % data
    serdev.write(data)

command = '%u[' % ptArray[chunkSize * int(frameStart / 1000)]

#swrite(command)
#time.sleep(1)
print 'Sleep 5'
time.sleep(5)
swrite('t')
time.sleep(1)

def fPrint(ii): print ii
def fSwrite(ii): swrite('C')
def fSleep1(ii): time.sleep(1)
def fUntension(ii): 
    if ((ii % 100) == 0):
        swrite('u')
        time.sleep(1)

funcTable = [ fPrint, fSwrite, fSleep1, fSleep1, fSleep1, fSleep1, fUntension ]
    
for ii in range(frameCount, frameEnd):
#    print "%s <- %s -> %s" % (frameCount, ii, frameEnd)
    for ff in funcTable:
        ff(ii)
        if serdev.inWaiting() > 0:
            sys.stdout.write("=> %s" % serdev.read(100))

swrite('x')
serdev.close()

#while ((count > 0))
#do
#    echo $count frames remaining
#    if [[ "$TRIPLE" == "" ]]
#    then
#        echo C | tr -d '\r\n'  > $USBDEV
#        sleep 2
#    else
#        echo 3 | tr -d '\r\n'  > $USBDEV
#        sleep 4
#    fi
#    ((count--))
#done
#echo x | tr -d '\r\n'  > $USBDEV
#
#serdev.close()

