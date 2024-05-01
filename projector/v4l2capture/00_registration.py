#!/usr/bin/env python3
# coding: utf-8

import  argparse
import cv2
from    glob import glob, iglob
import numpy as np
import os
from PIL import Image,ImageDraw,ImageFilter,ImageOps
import sys

def procargs():
    parser = argparse.ArgumentParser()
    parser.add_argument('--readfrom', dest='readfrom', help='read from glob', required=True)
    parser.add_argument('--writeto', dest='writeto', help='write to directory', required=True)
    return parser.parse_args()

def findSprocket(filename):
    original=cv2.imread(filename, cv2.IMREAD_ANYCOLOR)
    image = cv2.imread(filename,cv2.IMREAD_GRAYSCALE)
    image = image[:,0:int(image.shape[1]*0.15)]
    matrix = (3,7)
    sprocket = cv2.GaussianBlur(image,matrix,0)
    _,sprocket = cv2.threshold(sprocket,60,255,cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(sprocket, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contour = sorted(contours, key=lambda x: cv2.contourArea(x), reverse=True)[0] 
    area = cv2.contourArea(contour)
    rect = cv2.minAreaRect(contour)
    rotation = rect[2]
    centre = rect[0]
    colour = (100, 100, 100)
#    cv2.drawContours(sprocket, [contour], -1,color=colour, thickness=cv2.FILLED)
    box = np.intp(cv2.boxPoints(rect))
#    cv2.drawContours(sprocket, [box], -1,color=colour, thickness=2)
    avgright = (box[2][0]+box[3][0])/2
    avgleft = (box[0][0]+box[1][0])/2
    avgtop = (box[1][1]+box[2][1])/2
    avgbot = (box[0][1]+box[3][1])/2
    registration = int(avgright),int((avgtop+avgbot)/2)
#    cv2.circle(original,(int(avgright),int((avgtop+avgbot)/2)),12,(0,255,0),-1)
    return (registration,rotation)

def main():
    args = procargs()
    if not os.path.exists(os.path.dirname(args.readfrom)):
        print(f'{args.readfrom} does not exist')
        sys.exit(1)
    realpath = os.path.realpath(args.writeto)
    if not os.path.exists(os.path.dirname(realpath)):
        print(f'{args.writeto} does not exist')
        sys.exit(1)

    for file in sorted(glob(f'{args.readfrom}')):
        (reg,rot) = findSprocket(file)
        writeto = os.path.splitext(os.path.basename(file))[0]
        with open(f'{realpath}/{writeto}.reg','wb') as out:
            out.write(f'{reg[0]} {reg[1]} {rot}'.encode())

main()
