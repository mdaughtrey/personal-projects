#!/usr/bin/python3

import argparse
from glob import glob, iglob
import logging
from logging.handlers import RotatingFileHandler
import os
import pdb
import re
import serial
import signal
import subprocess
import sys
import threading
import time
import math
#import warnings
#warnings.filterwarnings('default',category=DeprecationWarning)

SerialPort="/dev/ttyACM0"
port = 0
tension = list()
lastTension = 0
numframes = 0


FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('etest')
fileHandler = logging.FileHandler(filename = './etest.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--ee', dest='e')
parser.add_argument('--frames', dest='frames', type=int, default=-1, help='frames to capture')
config = parser.parse_args()


def signal_handler(signal, frame):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    port.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)



def portWaitFor(port, text1):
    accum = b''
    logger.debug("Waiting on %s" % text1)
    while not text1 in accum:
        accum += port.read()
        #logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        logger.debug(f'Accumulated {accum}')
        return text1

def init():
    serPort = serial.Serial(SerialPort, 115200) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    serPort.write(b'33tl2d5D35E')
    return serPort

def frame(port, e):
    port.write(str.encode(f'?{e}emn'))
    portWaitFor(port, b'HDONE')
#    port.write(str.encode(f'?'))

def main():
    port = init()
    steps = config.e.split(',')
    for fr in range(config.frames):
        logger.debug(f'Frame {fr}')
        for f in steps:
            logger.debug(f'e{f}')
            frame(port,f)
            time.sleep(5)

main()

