#!/usr/bin/python3

import logging
import logging
from logging.handlers import RotatingFileHandler
from logging.handlers import RotatingFileHandler
import serial
import pdb

BIN='/home/mattd/personal-projects/projector/userland/build/bin/raspistill'
SPOOLTYPE="SMALL"
FILMTYPE="SUPER8"
PREFRAMES=1
FRAMES=0
SHUTTER=[40000,80000,1200000]
TARGETDIR='/tmp'
MAXINFLIGHT=30
SerialPort="/dev/ttyUSB0"

FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('rawcap')
fileHandler = logging.FileHandler(filename = './rawcap.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

def portWaitFor(port, text):
    accum = b''
    logger.debug("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
        logger.debug("Accumulating %s" % accum)
    logger.debug("Received %s" % text)
    return accum

def init():
    serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    serPort.write(b' ')
    portWaitFor(serPort, b'{State:Ready}')
    serPort.write(b'l')
#    serPort.write(b'c%st' % {'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]) 
    serPort.write("vc{0}t".format({'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]).encode('utf-8')) 
    portWaitFor(serPort, b'{pt:')
    return serPort

def stop(port):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    logger.debug("Closing")
    port.close()

def frame(port):
    for ss in SHUTTER:
        logger.debug("Shutter %d" % ss)
        port.write(b'n')
        portWaitFor(port, b'FRAMESTOP')

def main():
    port = init()
    frame(port)
    stop(port)

main()
