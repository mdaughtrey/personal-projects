#!/usr/bin/python3

import imageio

im = imageio.imread('reference0.ppm')
im.shape
imageio.imwrite('out.ppm', im[:,:,0])
