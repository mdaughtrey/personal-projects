#!/usr/bin/env python3
# coding: utf-8

import argparse
import cv2
from glob import glob, iglob
import logging
from   logging import FileHandler, StreamHandler
import numpy as np
import os
import pdb
from PIL import Image,ImageDraw,ImageFilter,ImageOps
import re
import sys

logger = None
args = None

def procargs():
    parser = argparse.ArgumentParser()
    parser.add_argument('--readfrom', dest='readfrom', help='read .reg files from directory', required=True)
    parser.add_argument('--writeto', dest='writeto', help='write rotated/cropped images to directory', required=True)
    parser.add_argument('--annotate', dest='annotate', help='annotate frames', action='store_true', default=False)
    parser.add_argument('--markonly', dest='markonly', help='mark frames', action='store_true', default=False)
    parser.add_argument('--exposures', dest='exposures', help='exposures', required=True)
    return parser.parse_args()

def setlogging(logname):
    global logger
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
#    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    
    logger = logging.getLogger('picam')
    logger.setLevel(logging.DEBUG)
    fileHandler = FileHandler(filename = logname)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

    stdioHandler = StreamHandler(sys.stdout)
    stdioHandler.setFormatter(logging.Formatter(fmt=FormatString))
    stdioHandler.setLevel(logging.INFO)
    logger.addHandler(stdioHandler)

#def getRect(regfile):
def getRect(leftX, centerY):
#    centerX, centerY, rotate = open(regfile.encode(),'rb').read().split(b' ')
    boxLeft = int(leftX) + 180
    boxRight = boxLeft + 1250
    boxTop = int(centerY) - 500
    boxBot = int(centerY) + 500
    #return boxLeft,boxRight,boxTop,boxBot,float(rotate)
    return boxLeft,boxRight,boxTop,boxBot

#def cropAndRotate(regfile, imagefile):
def cropAndRotate(leftX, centerY, readfrom, writeto):
    try:
        image = cv2.imread(readfrom, cv2.IMREAD_ANYCOLOR)
    except Exception as ee:
        logger.error(f'Error reading from {imagefile}: {str(ee)}')

    boxTop,boxBot,boxLeft,boxRight = getRect(leftX, centerY)
    height, width = image.shape[:2]
#    rMatrix = cv2.getRotationMatrix2D(center=(width/2,height/2),angle=rotate,scale=1)
#    rImage = cv2.warpAffine(src=image,M=rMatrix,dsize=(width,height))

    rImage = image
    if args.markonly:
#        cv2.circle(original,(int(avgright),int((avgtop+avgbot)/2)),12,(0,255,0),-1)
        rImage = cv2.rectangle(rImage, (boxTop, boxLeft), (boxBot, boxRight), (0,255,0), 1)
    else:
        rImage = rImage[boxLeft:boxRight, boxTop:boxBot]
    if args.annotate:
        org = (50,350)
        # write the text on the input image
        imageNum = os.path.splitext(os.path.basename(readfrom))[0].split('_')[0]
        cv2.putText(rImage, imageNum, org, fontFace = cv2.FONT_HERSHEY_COMPLEX, fontScale = 1.5, color = (250,225,100))
    cv2.imwrite(writeto, rImage)

def main():
    global args
    setlogging('01_crop_and_rotate.log')
    args = procargs()

#    readpath = os.path.realpath(args.readfrom)
    readpath = args.readfrom
    if not os.path.exists(os.path.dirname(readpath)):
        logger.error(f'{readpath} does not exist')
        sys.exit(1)

#    writepath = os.path.realpath(args.writeto)
    writepath = args.writeto
    if not os.path.exists(writepath):
        logger.info(f'Creating directory {writepath}')
        os.mkdir(writepath)

    exposures = [int(x) for x in args.exposures.split(',')]
#    centerX = None
    #for regfile in sorted(glob(f'{readpath}/*_{exposures[1]}.reg')):
    for regfile in sorted(glob(readpath)):
#        pdb.set_trace()
        rightX, leftX, centerY, _ = open(regfile.encode(),'rb').read().split(b' ')
#        if centerX is None:
#            centerX = cX
        for exposure in exposures[1:]:
            filename = os.path.basename(regfile).split('_')
            filename = f'{filename[0]}_{exposure}.png'
            readfrom = os.path.dirname(readpath) + '/' + filename
            writeto = writepath + '/' + filename
            #writeto = realpath + '/' +  os.path.splitext(os.path.basename(regfile))[0] + '.png'
            logger.debug(f'{filename}')
            if not os.path.exists(writeto):
                try:
                    cropAndRotate(int(leftX), int(centerY), readfrom, writeto)
                except:
                    logger.debug(f'Skipping {filename}')

main()
