#!/usr/bin/env python3

from glob import glob, iglob
from itertools import groupby
from PIL import Image, ImageDraw
import pdb
import scipy
from scipy import ndimage
import numpy as np

dodraw = False
# left upper right lower
crop = (540, 530, 3036, 2484)
PxPerMm8mm = 488
PxPerMmSuper8 = 391

def roundIt(val): return int(val + val%2)
SprocketSuper8 = type('obj', (object,), {'h': roundIt(.91 * PxPerMmSuper8),
    'w': roundIt(1.14 * PxPerMmSuper8)})
FrameSuper8 = type('obj', (object,), {'w': roundIt(5.46 * PxPerMmSuper8),
    'h': roundIt(4.01 * PxPerMmSuper8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'w': roundIt(4.5 * PxPerMm8mm),
    'h': roundIt(3.8 * PxPerMm8mm)})

def findExtents(matrix):
    (nrows, ncols) = matrix.shape
    test = np.full_like(matrix[0], 255)
    rstart = 0
    row = 0
    maxlen = 0
    for k, g in groupby(matrix, lambda x: np.array_equal(x, test)):
        leng = len(list(g))
        if k == True and leng > maxlen:
            maxlen = leng
            rstart = row
        row += leng
        
    return (0, rstart, ncols-1, rstart + maxlen)

def tupleAdd(t0, t1):
    return tuple(map(sum, (zip(t0, t1))))

# Shape = Y,X
def findSprockets(image):
    grey = image.convert('L')
    flattened = np.asarray(grey, dtype=np.uint8)
    sliceL = 550
    sliceR = 600
    slice = ndimage.grey_erosion(flattened[:,sliceL:sliceR], size=(25,25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

    threshold = darkest + (lightest - darkest)/2
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 255

    if dodraw: # sprocketslice
        # paste slice
        x = Image.fromarray(slice)
        rect =  (sliceL, 0, sliceR, x.size[1])
        image.paste(x, rect)
        # left upper right lower
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#0ff000', width=1)
        draw.rectangle(tupleAdd(rect, (1,1,-1,-1)), outline='#ffffff', width=1)

    upperSprocket = tupleAdd((sliceL, 0, sliceL, 0), findExtents(slice[:1300]))
    lowerSprocket = tupleAdd((sliceL, 2200, sliceL, 2200),
        findExtents(slice[2200:]))
    centerline = upperSprocket[3] + int((lowerSprocket[1] - upperSprocket[3])/2)
    croprect = (sliceR+150, centerline - (Frame8mm.h/2), sliceR + Frame8mm.w + 200,
            centerline + (Frame8mm.h/2))

    if dodraw:
        draw = ImageDraw.Draw(image)
        # upper sprocket
        draw.rectangle(upperSprocket, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(upperSprocket, (1,1,-1,-1)), outline='#ffffff', width=1)
        # lower sprocket
        draw.rectangle(lowerSprocket, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(lowerSprocket, (1,1,-1,-1)), outline='#ffffff', width=1)

        # Frame
        line = (0, centerline, image.size[0]-1, centerline)
        draw.line(line, fill=255, width=2)

        draw.rectangle(croprect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(croprect, (1,1,-1,-1)), outline='#ffffff', width=1)
    else:
        image = image.crop(croprect)

    return image

#for file in glob('/media/sf_vproj/scans/hqtest/*.rgb'):
for file in glob('/home/mattd/Documents/hqtest/*.rgb'):
    print(file)
    image = Image.frombytes('RGB', (4064, 3040),open(file,'rb').read())
    if dodraw:
        draw = ImageDraw.Draw(image)
        draw.rectangle(crop, outline='#000000', width=1)
        draw.rectangle(tupleAdd(crop, (-1,-1,1,1)), outline='#ffffff', width=1)

    image = findSprockets(image)
    image.save(file.replace('.rgb', '_cropped.png'))
    #image.save(file.replace('.rgb', '_cropped.yuv2'))
#    image.convert('YCbCr').save(file.replace('.rgb', '_cropped.yuv'), format='YCbCr')
