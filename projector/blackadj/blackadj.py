#!/usr/bin/python3

import numpy
import cv2
import imageio
from PIL import Image
import pdb

uncorr = cv2.imread("./test.ppm")
#pdb.set_trace()
rmin = numpy.amin(uncorr[:,0])
gmin = numpy.amin(uncorr[:,1])
bmin = numpy.amin(uncorr[:,2])
anew=uncorr - [rmin,bmin,gmin]
imageio.imwrite("test.png", anew)
