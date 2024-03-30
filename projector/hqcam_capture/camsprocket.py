#!/bin/env python3
# CamSprocket - position each frame by examine its sprocket position via the camera
# References: 
# https://github.com/raspberrypi/picamera2/tree/main/picamera2

import argparse
import cv2
from  glob import glob, iglob
from libcamera import Transform
import logging
from  logging.handlers import RotatingFileHandler
from matplotlib import pyplot as plt
import numpy as np
import os
import pdb
from picamera2 import Picamera2, Preview
from PIL import Image, ImageOps
from scipy import ndimage
import serial
import sys
from Tension import Tension
import time

serdev = None
logger = None

class SerDev():
    def __init__(self, config):
        self.serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
#        logger = logging.getLogger('serdev')
#        logger.setLevel(logging.DEBUG)
        if self.serdev.isOpen():
            self.serdev.close()
        self.serdev.open()
        self.write(b' ')

    def write(self, message):
        logger.debug(message)
        self.serdev.write(message)

    def waitfor(self, text1, text2):
        accum = b''
        logger.debug("Waiting on %s or %s" % (text1, text2))
        while not text1 in accum and not text2 in accum:
            try:
                accum += self.serdev.read()
            except:
                logger.debug("serwaitfor timeout")
        if text1 in accum:
            logger.debug("Matched on %s" % text1)
            return (0, text1, accum)
        if text2 in accum:
            logger.debug("Matched on %s" % text2)
            return (1, text2, accum)

    def waitready(self):
        return self.waitfor(b'{State:Ready}', b'No')

def args_framecap():
    parser = argparse.ArgumentParser()
#    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument(dest='do')
    parser.add_argument('--debug', dest='debug', action='store_true', help='debug (no crop, show lines)')
    parser.add_argument('--showwork', dest='showwork', action='store_true', help='show intermediary images')
    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
    parser.add_argument('--fastforward', dest='fastforward', type=int, help='fast forward multiplier', default=8)
    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8', required=True)
    parser.add_argument('--frames', dest='frames', type=int, default=-1, help='Number of frames to capture')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', required=True, help='Log file')
    parser.add_argument('--optocount', dest='optocount', type=int, default=int(steps_per_frame/10), help='optocount per frame')
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    parser.add_argument('--exposure', dest='exposure', help='EDR exposure a,b,[c,..]')
    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
    return parser.parse_args()

def setlogging(config):
    global logger
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('camsprocket')
    fileHandler = logging.FileHandler(filename = config.logfile)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

def findSprocket(image, show=False):
    y,x = image.shape[:2]
    if show:
        plt.imshow(image)
        plt.show()
    image = image[int(y/3):y-int(y/3),0:int(x/4)]
    if show:
        plt.imshow(image)
        plt.show()
    image2 = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    image3 = np.asarray(image2, dtype=np.uint8)
    image3 = ndimage.grey_erosion(image3, size=(5,5))
    _, image3 = cv2.threshold(image3, 100, 255, cv2.THRESH_BINARY)
    if show:
        plt.imshow(image3,cmap='gray')
        plt.show()

    def whtest(contour):
        (x,y,w,h) = cv2.boundingRect(contour)
        return (50 < w < 60) & (60 < h < 70)

    # Find the contours in the thresholded image
    contours, _ = cv2.findContours(image3, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours = list(filter(whtest, contours))
    if 1 != len(contours):
        return False
    if show == False:
        return True

    for c in contours:
        print('Contour Area: ' + str(cv2.contourArea(c)))
        x, y, width, height = cv2.boundingRect(c)
        print(f'x {x} y {y} width {width} height {height}')
    contour = contours[0]

    # Get the bounding box of the largest contour
    x, y, width, height = cv2.boundingRect(contour)

    # Print the size and location of the white square
    print(f'White square size: {width}x{height} pixels')
    print(f'White square location: ({x}, {y})')
    cv2.drawContours(image3, [contour], -1, (100,100,100), thickness=cv2.FILLED)
    plt.imshow(image3,cmap='gray')
    plt.show()

    return True

def init_framecap(config):
#    pdb.set_trace()
    if config.showwork and not os.path.exists(workdir:='{}/work'.format(config.framesto)):
        os.mkdir(workdir)
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    tension = np.multiply(np.array(tension), 9)
    numframes += 1000
#    logger = logging.getLogger('framecap')
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))

    global lastTension
    lastTension = tension[0]

    logger.info("Opening %s" % config.serdev)
    global serdev
    serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
    if serdev.isOpen():
        serdev.close()
    serdev.open()
    serdev.write(b' ')
    wait = serdev.waitfor(b'{State:Ready}', b'No')
    # k - echo
    # c - clear
    # v - verbose
    # t - tension
    # l - lamp
    # e - encoder limit
    # c - clear param
    # E - encodeer slow threshold 8000
    # o - timeout
    # C - stepper menu
    # - c - clear param
    # - I - min interval 500
    # - c - clear param
    # - i - max interval 2000
    # - c - clear param
    # - D - ramp down 25
    # - U - ramp up 25
    # - x - return to main
    #message = f'kcv{lastTension}tl{config.optocount}ecE8000oCc500Ic2000ic25D25Ux'.encode()          
    serdev.write(b'c120t')
    time.sleep(1.0)
    message = f'Wcv{lastTension}tl'.encode()
    serdev.write(message)

def main():
    global config
    config = args_framecap()
    setlogging(config)
    init_framecap(config)

main()

#picam = Picamera2()
#config = picam.create_video_configuration(main={'size': (2304,1296), "format":"RGB888"},
#                                          transform = Transform(hflip=True),
#                                          lores={"size":(640,480),"format":"RGB888"},
#                                          controls={'FrameDurationLimits':(1000000,1000000),
#                                                    'ExposureTime':22000})
#picam.configure(config)
#picam.align_configuration(config)
#picam.start()
#
#while True:
#    buffer = picam.capture_array("lores")
#    good = findSprocket(buffer,show = False)
##    picam.stop()


#pdb.set_trace()
#for image in sorted(glob('/media/frames/frames_for_cam_sprocket/*.png')):
#    print(image + ' ' + str(findSprocket(cv2.imread(image), show = False)))
#image = cv2.imread('/media/frames/frames_for_cam_sprocket/0877.png')
#cProfile.run("findSprocket(image)")

