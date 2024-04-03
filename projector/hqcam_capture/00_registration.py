#!/usr/bin/env python3
# coding: utf-8

import  argparse
import cv2
from    glob import glob, iglob
import  logging
from   logging import FileHandler, StreamHandler
import numpy as np
import os
from PIL import Image,ImageDraw,ImageFilter,ImageOps
import pdb
from    scipy import ndimage
import sys

args = None
logger = None

def procargs():
    parser = argparse.ArgumentParser()
    parser.add_argument('--readfrom', dest='readfrom', help='read from glob')
    parser.add_argument('--onefile', dest='onefile', help='process one file')
    parser.add_argument('--writeto', dest='writeto', help='write to directory', required=True)
    parser.add_argument('--debugto', dest='debugto', help='save intermediary images to directory')
    parser.add_argument('--webdb', dest='webdb', help='enable web debugger (port 5555)', action='store_true', default=False)
    return parser.parse_args()

def setlogging():
    global logger
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
#    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    
    logger = logging.getLogger('picam')
    logger.setLevel(logging.DEBUG)
    fileHandler = FileHandler(filename = 'registration.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

    stdioHandler = StreamHandler(sys.stdout)
    stdioHandler.setFormatter(logging.Formatter(fmt=FormatString))
    stdioHandler.setLevel(logging.INFO)
    logger.addHandler(stdioHandler)

def findSprocket(filename):
#    if args.webdb:
#        import web_pdb
    def writeDebugFile(tag, image):
        outfile = os.path.splitext(os.path.basename(filename))[0]
        outfile = f'{args.debugto}/{outfile}_{tag}.png'
        cv2.imwrite(outfile, image)

    debugout = args.debugto #  and -1 != filename.find('36000')
    original=cv2.imread(filename, cv2.IMREAD_ANYCOLOR)
    image = cv2.imread(filename,cv2.IMREAD_GRAYSCALE)
    image = image[:,0:int(image.shape[1]*0.15)]

    image = np.asarray(image, dtype=np.uint8)
    image = ndimage.grey_erosion(image, size=(5,5))

    if debugout:
        writeDebugFile('slice', image)

    matrix = (3,7)
    sprocket = cv2.GaussianBlur(image,matrix,0)
    _,sprocket = cv2.threshold(sprocket,60,255,cv2.THRESH_BINARY)

    if debugout:
        writeDebugFile('sprocket', sprocket)

    contours, _ = cv2.findContours(sprocket, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    contour = sorted(contours, key=lambda x: cv2.contourArea(x), reverse=True)[0] 
    area = cv2.contourArea(contour)
    # ((centerX, centerY),(dimX,dimY),angle)
    rect = cv2.minAreaRect(contour)
    logger.debug(f'{os.path.basename(filename)}: {rect}')
    if rect[2] < 10.0:
        rect = (rect[0], (rect[1][1], rect[1][0]), rect[2] + 90.0)
    rotation = rect[2]
    centre = rect[0]
    if debugout:
        color = (100, 100, 100)
        s2 = sprocket.copy()
        cv2.drawContours(s2, [contour], -1,color=color, thickness=cv2.FILLED)
        writeDebugFile('contours', s2)

    # Top Left (x,y), Top Right (x,y), Bottom Right (x,y), Bottom Left (x,y)
    box = np.intp(cv2.boxPoints(rect))
    if debugout:
        s2 = sprocket.copy()
        color = (100, 100, 100)
        cv2.drawContours(s2, [box], -1,color=color, thickness=2)
        writeDebugFile('boxed', s2)

    avgright = (box[1][0]+box[2][0])/2
    avgleft = (box[0][0]+box[3][0])/2
    avgtop = (box[0][1]+box[1][1])/2
    avgbot = (box[2][1]+box[3][1])/2
    registration = int(avgright),int(avgleft),int((avgtop+avgbot)/2)
    if debugout:
        o2 = original.copy()
        cv2.circle(o2,(int(avgright),int((avgtop+avgbot)/2)),12,(0,255,0),-1)
        writeDebugFile('circle', o2)
    return (registration,rotation)

def main():
    global args
    args = procargs()
    setlogging()
    if args.readfrom and not os.path.exists(os.path.dirname(args.readfrom)):
        logger.error(f'{args.readfrom} does not exist')
        sys.exit(1)

    if args.onefile and not os.path.isfile(args.onefile):
        logger.error(f'{args.onefile} does not exist')
        sys.exit(1)

    realpath = os.path.realpath(args.writeto)
    if not os.path.exists(os.path.dirname(realpath)):
        logger.error(f'{args.writeto} does not exist')
        sys.exit(1)

    if args.onefile:
        (reg,rot) = findSprocket(args.onefile)
        writeto = os.path.splitext(os.path.basename(args.onefile))[0]
        with open(f'{realpath}/{writeto}.reg','wb') as out:
            out.write(f'{reg[0]} {reg[1]} {rot}'.encode())

    for file in sorted(glob(args.readfrom)):
        (reg,rot) = findSprocket(file)
        writeto = os.path.splitext(os.path.basename(file))[0]
        with open(f'{realpath}/{writeto}.reg','wb') as out:
            out.write(f'{reg[0]} {reg[1]} {reg[2]} {rot}'.encode())

main()
