#!/bin/env python3

import cv2 as cv
import glob
import numpy as np
import os
import pdb
import sys

def dw(name, data):
    cv.imwrite(f'{name}.png', np.asarray(data))

def main():
    frames = 'frames/outside/capture'
    if not os.path.exists(frames):
        print('{} not found'.format(frames))
        sys.exit(1)

    graded = 'frames/outside/graded'
    if not os.path.exists(graded):
        os.mkdir(graded)

#    pdb.set_trace()
    reference = cv.imread('frames/outside/capture/00000002.png').astype(np.float32)
    gray = np.array([128.0,128.0,128.0])
#    black = np.array([0.0,0.0,0.0])
#    white = np.array([255.0,255.0,255.0])
#    pdb.set_trace()
    ref0 = np.clip(reference - gray, 0, 255)
    cv.imwrite(f'subtract.png', np.asarray(ref0))
    ref0 = np.clip(reference / gray, 0, 255)
    cv.imwrite(f'divide.png', np.asarray(ref0))
    ref0 = np.clip(reference * gray, 0, 255)
    cv.imwrite(f'multiply.png',np.asarray(ref0))

    pdb.set_trace()
    for file in glob.glob('{}/????????.png'.format(frames)):
        #image=cv.imread(file).astype(np.float32)
        thisfile=cv.imread(file).astype(np.float32)
        image=thisfile-reference
        #image=np.clip(image / ref0, 0, 255)
        cv.imwrite('{}/{}'.format(graded, os.path.basename(file)), np.asarray(image))

main()
