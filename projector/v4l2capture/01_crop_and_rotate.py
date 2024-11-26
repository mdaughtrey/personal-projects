#!/usr/bin/env python3
# coding: utf-8

import argparse
import cv2
from glob import glob, iglob
import logging
import numpy as np
import os
import pdb
from PIL import Image,ImageDraw,ImageFilter,ImageOps
import sys

logger = None
args = None

def procargs():
    parser = argparse.ArgumentParser()
    parser.add_argument('--readfrom', dest='readfrom', help='read .reg files from directory', required=True)
    parser.add_argument('--writeto', dest='writeto', help='write rotated/cropped images to directory', required=True)
    parser.add_argument('--annotate', dest='annotate', help='annotate frames', action='store_true', default=False)
    return parser.parse_args()

def setlogging(logname):
    global logger
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('01_crop_and_rotate')
    fileHandler = logging.FileHandler(filename = logname)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

def getRect(regfile):
    centerX, centerY, rotate = open(regfile.encode(),'rb').read().split(b' ')
    boxLeft = int(centerX)
    boxRight = boxLeft + 1700
    boxTop = int(centerY) - 630
    boxBot = int(centerY) + 630
    return boxLeft,boxRight,boxTop,boxBot,float(rotate)

def cropAndRotate(regfile, imagefile):
    try:
        image = cv2.imread(imagefile, cv2.IMREAD_ANYCOLOR)
    except Exception as ee:
        logger.error(f'Error reading from {imagefile}: {str(ee)}')
        return False, None
    boxTop,boxBot,boxLeft,boxRight,rotate = getRect(regfile)
    height, width = image.shape[:2]
#    rMatrix = cv2.getRotationMatrix2D(center=(width/2,height/2),angle=rotate,scale=1)
#    rImage = cv2.warpAffine(src=image,M=rMatrix,dsize=(width,height))
    imageNum = os.path.splitext(os.path.basename(imagefile))[0].split('_')[0]

    rImage = image
    rImage = rImage[boxLeft:boxRight, boxTop:boxBot]
    if args.annotate:
        org = (50,350)
        # write the text on the input image
        cv2.putText(rImage, imageNum, org, fontFace = cv2.FONT_HERSHEY_COMPLEX, fontScale = 1.5, color = (250,225,100))
    return True, rImage

def main():
    global args
    setlogging('01_crop_and_rotate.log')
    args = procargs()
    if not os.path.exists(os.path.dirname(args.readfrom)):
        print(f'{args.readfrom} does not exist')
        sys.exit(1)

    realpath = os.path.realpath(args.writeto)
    if not os.path.exists(realpath):
        print(f'Creating directory {realpath}')
        os.mkdir(realpath)

    for regfile in sorted(glob(f'{args.readfrom}/*.reg')):
        writeto = realpath + '/' +  os.path.splitext(os.path.basename(regfile))[0] + '.png'
        logger.debug(f'Writing to {writeto}')
        if not os.path.exists(writeto):
            good, image = cropAndRotate(regfile, regfile.replace('.reg','.png'))
            if not good:
                continue
            try:
                cv2.imwrite(writeto,image)
            except Exception as ee :
                logger.error(f'Crapped out writing {writeto}: {str(ee)})')


main()
