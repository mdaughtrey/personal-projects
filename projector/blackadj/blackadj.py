#!/usr/bin/python3

import numpy
import cv2
import imageio
from PIL import Image
import pdb

uncorr = cv2.imread("./test.ppm")
pdb.set_trace()
imageio.imwrite("test.png", uncorr)
