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
logger = logging.getLogger('hqcap')

parser = argparse.ArgumentParser()
parser.add_argument('--ee', dest='e')
config = parser.parse_args()


def signal_handler(signal, frame):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    port.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)



def init():
    serPort = serial.Serial(SerialPort, 115200) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    serPort.write(b'33tl2d5D75E')
    return serPort

def frame(port, e):
        port.write(str.encode(f'{e}emn'))

def main():
    port = init()
    steps = config.e.split(',')
    while True:
        for f in steps:
            logger.debug(f'e{f}')
            frame(port,f)
            time.sleep(5)

main()

