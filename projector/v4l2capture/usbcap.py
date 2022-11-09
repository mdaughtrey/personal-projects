#!/usr/bin/env python3
#./opencvcap.py --serialport /dev/ttyACM0 --camindex 0 --film s8 --framesto ~/share/opencvcap0 --startdia 140 --enddia 80 --res draft

import argparse
import cv2
from glob import glob, iglob
import logging
from logging.handlers import RotatingFileHandler
import math
import os
import pdb
import serial
from Tension import Tension
import time

res = [(640,480),(3264,2448),(2592,1944),(1920,1080),(1600,1200),(1280,720),(960,540),(848,480),(640,360),(424,240),(320,240),(320,180),(640,480)]


def proc_commandline():
    parser = argparse.ArgumentParser()
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8')
    parser.add_argument('--frames', dest='frames', type=int, default=-1, help='Number of frames to capture')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
#    parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
    return parser.parse_args()

def setlogging(config):
    FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('hqcap')
    fileHandler = logging.FileHandler(filename = './usbap.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)
    return logger

def serwaitfor(port, text1, text2, logger):
    accum = b''
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        try:
            accum += port.read()
        except:
            logger.debug("serwaitfor timeout")
        logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return (0, text1)
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return (1, text2)

def init(config, logger):
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    numframes += 1000
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))
    map (lambda x: x * 3, tension)

    global lastTension
    lastTension = tension[0]

    logger.debug("Opening %s" % config.serdev)
    serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
    if serdev.isOpen():
        serdev.close()
    serdev.open()
    serdev.write(b' ')
    wait = serwaitfor(serdev, b'{State:Ready}', b'No', logger)
#    if b'{State:Ready}' != portWaitFor(serPort, b'{State:Ready}'):
#        logger.debug("Init failed")
#        sys.exit(1)

    interval = 20
    serdev.write(str.encode(f'{lastTension}tl1d2D{interval}e0E6000o'))
#    portWaitFor(serPort, b'{State:Ready}')
#    if False == config.noled:
#        serPort.write(b'l')
#    serPort.write(b'c%st' % {'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]) 
#    serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film], tension[0]).encode('utf-8'))
    #serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film].encode('utf-8'), tension[0]))
#    portWaitFor(serPort, b'{pt:')
    return serdev

def get_most_recent_frame(config):
    files = sorted(glob("{0}/*.bmp".format(config.framesto)))
    if len(files):
        frames =  [int(os.path.basename(os.path.splitext(f)[0])) for f in files]
        frame = 1+ sorted(frames)[::-1][0]
    else:
        frame = 0
    return frame


def framecap(config, logger, serdev):
    framenum = get_most_recent_frame(config)
    cap = cv2.VideoCapture(config.camindex)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, res[config.res][0])
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, res[config.res][1])
    pdb.set_trace()
    for x in range(1,10):
        ret, frame = cap.read()
        cv2.imwrite("{}/{:>08}.bmp".format(config.framesto, framenum), frame)
        framenum += 1
        time.sleep(1.0)
        serdev.write(b'n')
        wait = serwaitfor(serdev, b'{HDONE}', b'{NTO}', logger)
    cap.release()


def main():
    config = proc_commandline()
    logger = setlogging(config)
    serdev = init(config, logger)
    framecap(config, logger, serdev)

main()
