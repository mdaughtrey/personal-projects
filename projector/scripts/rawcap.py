#!/usr/bin/python3

import logging
import logging
import signal
from logging.handlers import RotatingFileHandler
from logging.handlers import RotatingFileHandler
import serial
import time
import subprocess
import pdb
import re
import os
from glob import glob, iglob
import argparse

BIN='/home/mattd/personal-projects/projector/userland/build/bin/raspistill'
SPOOLTYPE="SMALL"
FILMTYPE="SUPER8"
PREFRAMES=1
FRAMES=0
#SHUTTER=[40000,80000,1200000]

SHUTTER=[6, 57, 57*4]
#SHUTTER=[10,150,600]
TARGETDIR='/tmp'
MAXINFLIGHT=30
SerialPort="/dev/ttyUSB0"
OUTPUTDIR="/mnt/extfd"
#OUTPUTDIR="/home/mattd/capture"

FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('rawcap')
fileHandler = logging.FileHandler(filename = './rawcap.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--frames', dest='frames', type=int, default=-1, help='frames to capture')
parser.add_argument('--dev', dest='dev', help='image device')
parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
parser.add_argument('--nofilm', dest='nofilm', action='store_true', default=False, help='run with no film loaded')
parser.add_argument('--noled', dest='noled', action='store_true', default=False, help='run with no LED')
config = parser.parse_args()

def sig(signum, frame):
    print("Sig handler")
signal.signal(signal.SIGTERM, sig)

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
    if False == config.noled:
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
    if False == config.nofilm:
        port.write(b'n')
        if b'{OIT:' == portWaitFor2(port, b'FRAMESTOP', b'{OIT:'):
            return 
    for ss,tag in zip(SHUTTER, ['a','b','c']):
#    for ss in range(1, 100000, 1000):
        logger.debug("Frame {0} shutter {1} tag {2}".format(num, ss, tag))
        runargs = ("/usr/local/bin/raspiraw --mode 0 --header --i2c 0 --expus {0} ".format(ss),
                "--fps 1 -t 100 -sr 1 -o {:s}/{:s}{:06d}{:s}.raw".format(OUTPUTDIR, config.prefix, num, tag))
        #runargs = ("/usr/local/bin/raspiraw --mode 0 --header --i2c 0 --expus {0} ".format(ss),
        #        "--fps 1 -t 250 -sr 1 -w 640 -h 480 -o {:s}/{:06d}.raw".format(OUTPUTDIR, num))
        logger.debug(''.join(runargs))
        retcode = subprocess.call(''.join(runargs), shell=True, stderr=None)
        logger.debug("retcode %u" % retcode)
        open("{:s}/{:06d}{:s}.done".format(OUTPUTDIR, num, tag), "w")
        #time.sleep(2)
        #num += 1

def main():
#    subprocess.call('/usr/local/bin/camera_i2c', shell=True, stderr=None)
    files = sorted(glob("{0}/{1}??????.done".format(OUTPUTDIR, config.prefix)))
    if len(files):
        frameNum = int(files[-1][-11:-5])
    else:
        frameNum = 0
    logger.debug("Starting at frame {0}".format(frameNum))
    port = init()
    frameCount = 0
    for frameCount in range(0, config.frames):
        while True:
            statvfs = os.statvfs(config.dev)
            if 20e6 < statvfs.f_frsize * statvfs.f_bavail:
                break;
            else:
                logger.info('Waiting for disk space')
                time.sleep(10)
        frame(port, frameNum)
        frameNum += 1
    stop(port)
    open("{:s}/{:s}done.done".format(OUTPUTDIR, config.prefix), "w")

main()

