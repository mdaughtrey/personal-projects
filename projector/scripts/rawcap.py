#!/usr/bin/python3

import logging
import logging
from logging.handlers import RotatingFileHandler
from logging.handlers import RotatingFileHandler
import serial
import subprocess
import pdb
import re
from glob import glob, iglob

BIN='/home/mattd/personal-projects/projector/userland/build/bin/raspistill'
SPOOLTYPE="SMALL"
FILMTYPE="SUPER8"
PREFRAMES=1
FRAMES=0
SHUTTER=[40000,80000,1200000]
TARGETDIR='/tmp'
MAXINFLIGHT=30
SerialPort="/dev/ttyUSB0"
#OUTPUTDIR="/tmp"
OUTPUTDIR="/home/mattd/capture"

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
        #logger.debug("Accumulating %s" % accum)
    logger.debug("Received %s" % text)
    return accum

def portWaitFor2(port, text1, text2):
    accum = b''
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        accum += port.read()
        #logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return text1
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return text2

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

def frame(port, num):
    port.write(b'n')
    if b'{OIT:' == portWaitFor2(port, b'FRAMESTOP', b'{OIT:'):
        return 
    for ss in SHUTTER:
        logger.debug("Frame {0} shutter {1}".format(num, ss))
        runargs = ("/usr/local/bin/raspiraw --mode 0 --header --i2c 0 --expus {0} ".format(ss),
                "--fps 1 -t 250 -sr 1 -o {0}/cap{1}.raw".format(OUTPUTDIR, num))
        #runargs = ("/usr/local/bin/raspiraw")
        subprocess.call(''.join(runargs), shell=True)
        num += 1

def main():
    pdb.set_trace()
    files = sorted(glob("{0}/cap*.raw".format(OUTPUTDIR)))
    matched = re.search("{0}/cap([0-9]+).raw".format(OUTPUTDIR), files[-1])
    frameNum = int(matched.group(1)[0])
    port = init()
    while True:
        frame(port, frameNum)
        frameNum += 1
    stop(port)

main()
