#!/usr/bin/python3

import numpy
import imageio
from PIL import Image
import pdb

uncorr = imageio.imread("./test.ppm")
imageio.imwrite("test.png", uncorr)
