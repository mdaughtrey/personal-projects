#!/usr/bin/python3

import numpy
import imageio
from PIL import Image
import pdb

refFile = imageio.imread("./reference0.ppm")
rgbRef = numpy.full(refFile.shape, 255, dtype=numpy.uint8)
imageio.imwrite("ref.png", rgbRef)
delta = rgbRef - refFile
result = refFile + delta
imageio.imwrite("result.png", result)


