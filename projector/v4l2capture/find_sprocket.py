#!/usr/bin/env python3
# coding: utf-8

import cv2
import numpy as np
import os
import pdb
from PIL import Image,ImageDraw,ImageFilter,ImageOps

testfile='/frames/20240207_test1/capture/00000100_2000.png'


def findSprocket(filename):
    original=cv2.imread(testfile, cv2.IMREAD_ANYCOLOR)
    image = cv2.imread(testfile,cv2.IMREAD_GRAYSCALE)
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
    cv2.drawContours(sprocket, [contour], -1,color=colour, thickness=cv2.FILLED)
    box = np.intp(cv2.boxPoints(rect))
    cv2.drawContours(sprocket, [box], -1,color=colour, thickness=2)
    avgright = (box[2][0]+box[3][0])/2
    avgleft = (box[0][0]+box[1][0])/2
    avgtop = (box[1][1]+box[2][1])/2
    avgbot = (box[0][1]+box[3][1])/2
    registration = int(avgright),int((avgtop+avgbot)/2)
    cv2.circle(original,(int(avgright),int((avgtop+avgbot)/2)),12,(0,255,0),-1)
    return (registration,rotation)

(reg,rot) = findSprocket(testfile)
pdb.set_trace()

