#!/usr/bin/python

import serial
import time
import sys
import pdb
import logging
from optparse import OptionParser
options = {}
args = ()

logging.

def getOptions():
    global options
    global args
    parser = OptionParser('cycle.py [-v] -i inputdir -o outputdir')
    parser.add_option('-s', '--startframe', dest='startframe')
    parser.add_option('-e', '--endframe', dest='endframe')
    parser.add_option('-c', '--cameradir', dest='cameradir')
    parser.add_option('-t', '--targetdir', dest='targetdir')
    parser.add_option('-p', '--pretension', dest='pretension')
    parser.add_option('-r', '--servo', dest='servo')
    parser.add_option('-m', '--mode', dest='mode', help='super8|8mm')
    parser.add_option('-d', '--serdevice', dest='serialdevice', help='Serial device',
        default='/dev/tty.usbserial-A601KW2O')

    (options, args) = parser.parse_args()

def main():
    getOptions()

if len(sys.argv) != 4:
    print "Usage: cycle.py framestart frameend pretension"
    sys.exit(1)

serdev = serial.Serial(USBDEV, 57600, timeout=1.0)





time.sleep(2)
(frameStart, frameEnd) = [int(xx) for xx in sys.argv[1:3]]
frameCount = frameStart

def swrite(data):
#    print "Serial: %s" % data
    serdev.write(data)

#command = '%u[-' % ptArray[chunkSize * int(frameStart / 1000)]

#time.sleep(1)
print 'Sleep 5'
time.sleep(5)
print 'Set pretension %s' % sys.argv[3]
command = '%s[-' % sys.argv[3]
swrite(command)
swrite('t')
time.sleep(1)

def fPrint(ii): print ii
def fSwrite(ii): swrite('C')
def fNext(ii): swrite('n')
def fSleep1(ii):
    print 'Sleep 1'
    time.sleep(1)
#def fUntension(ii): 
#    if ((ii % 100) == 0):
#        swrite('u')
#        time.sleep(1)

#funcTable = [ fPrint, fSwrite, fSleep1, fSleep1 ]
funcTable = [ fPrint, fSwrite, fSleep1, fSleep1, fSleep1, fSleep1, fSleep1 ]
#funcTable = [ fPrint, fNext, fSleep1, fSleep1, fSleep1, fSleep1, fUntension ]
    
for ii in range(frameCount, frameEnd):
#    print "%s <- %s -> %s" % (frameCount, ii, frameEnd)
    for ff in funcTable:
        ff(ii)
            #sys.stdout.write("=> %s" % serdev.read(100))

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

