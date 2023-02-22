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
    frames = 'frames/outside'
    if not os.path.exists(frames):
        print('{} not found'.format(frames))
        sys.exit(1)

    graded = 'frames/outside/graded'
    if not os.path.exists(graded):
        os.mkdir(graded)

    reference = cv.imread('reference.png').astype(np.float32)
    gray = np.array([128.0,128.0,128.0])
    ref0 = np.clip(reference / gray, 0, 255)
    cv.imwrite(f'graderef.png', np.asarray(ref0))

    for file in glob.glob('{}/????????.png'.format(frames)):
        image=cv.imread(file).astype(np.float32)
        image=np.clip(image / ref0, 0, 255)
        cv.imwrite('{}/{file}'.format(graded, os.path.basename(file)), np.asarray(image))

main()
