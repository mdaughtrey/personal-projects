#!/usr/bin/env python3
# coding: utf-8

import  argparse
import cv2
from    glob import glob, iglob
import  logging
from   logging import FileHandler, StreamHandler
import numpy as np
import os
from picam_utils import *
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
    parser.add_argument('--webdb', dest='webdb', help='enable web debugger (port 5555)', action='store_true', default=False)
    return parser.parse_args()


def main():
    global args
    global logger
    args = procargs()
    logger = setLogging('registration', '00_registration.log', logging.INFO)['logger']
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
        pdb.set_trace()
        (_,x,y,width,height) = findSprocket(logger, cv2.imread(args.onefile, cv2.IMREAD_ANYCOLOR), hires=True, savework=True)
        writeto = os.path.splitext(os.path.basename(args.onefile))[0]
        with open(f'{realpath}/{writeto}.reg','wb') as out:
            out.write(f'{x+width} {x} {int(y+height/2)}'.encode())
        return

    for file in sorted(glob(args.readfrom)):
        (_,x,y,width,height) = findSprocket(logger, cv2.imread(file, cv2.IMREAD_ANYCOLOR), hires=True)
        writeto = os.path.splitext(os.path.basename(file))[0]
        with open(f'{realpath}/{writeto}.reg','wb') as out:
            out.write(f'{x+width} {x} {int(y+height/2)}'.encode())

main()
