#!/usr/bin/python3

import scipy
import struct
from scipy import ndimage
from scipy import signal
from optparse import OptionParser
from PIL import Image as PILImage, ImageDraw, ImageFilter, ImageFile
#import Image
# import Image, ImageDraw, ImageFilter
import sys
import os
import pdb
import numpy
from operator import itemgetter
from itertools import groupby
from glob import glob, iglob
import logging
from logging.handlers import RotatingFileHandler
import cv2
#from cv2 import matchTemplate as cv2m
from collections import namedtuple
from operator import mul
import timeit
import copy
import subprocess

ImageFile.LOAD_TRUNCATED_IMAGES = True
XFudge = 225

options = {}

parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')
parser.add_option('-d', '--debug', dest='debug', help='Save intermediary images', action='store_true')
parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False)
parser.add_option('-w', '--whitecount', dest='whitecount', action='store_true', default=False)
parser.add_option('-i', '--input-dir', dest='inputdir')
parser.add_option('-o', '--output-dir', dest='outputdir')
parser.add_option('-f','--filenames', dest='filenames', help='Three filenames, comma separated')
parser.add_option('-m','--mode', dest='mode', default='8mm')
parser.add_option('-l','--listfile', dest='listfile', help='file containing list of triplets')
parser.add_option('-a','--adjfile', dest='adjfile', help='frame adjustmentfile (x,y,w,h)')
parser.add_option('-s','--single', dest='single', action='store_true', default=False, help='one image per frame')

(options, args) = parser.parse_args()

sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('%s/sprockets' % options.outputdir)]
eroded_dir = {True:'eroded', False: None}[os.path.isdir('%s/eroded' % options.outputdir)]
PxPerMm8mm = 391
PxPerMmSuper8 = 391
Info = namedtuple('Info', 'start height')
 
def roundIt(val): return int(val + val%2)
SprocketSuper8 = type('obj', (object,), {'h': roundIt(.91 * PxPerMmSuper8),
    'w': roundIt(1.14 * PxPerMmSuper8)})
FrameSuper8 = type('obj', (object,), {'w': roundIt(5.46 * PxPerMmSuper8),
    'h': roundIt(4.01 * PxPerMmSuper8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'w': roundIt(4.37 * PxPerMm8mm),
    'h': roundIt(3.28 * PxPerMm8mm)})

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(message)s')
logger = logging.getLogger('autocrop')
fileHandler = RotatingFileHandler(filename='./autocroph.log', maxBytes=10e6, backupCount=2)
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

def whitePixels(data):
    whites = []

    idx = 0
    for kk, gg in groupby(data, lambda x: x):
        count = sum(1 for _ in gg)
        #if 1 == kk:
        if 255 == kk:
            whites.append([idx, count])
        idx += count
    return whites

def processSuper8(filenames, outputpath):
    files = filenames.split(',')
    if 3 != len(files):
        logger.error("Need three filenames")
        sys.exit(1)

    # Select the brightest image for figuring out the cropping
    filename = files[1]
    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    if options.debug and eroded_dir is not None:
        sPlacement = imp
    (fcHeight, fcWidth) = flattened.shape
    sprocket = ndimage.grey_erosion(flattened[:,:300], size=(25, 25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(sprocket)
    lightest = ndimage.maximum(sprocket)

    threshold = darkest + (lightest - darkest)/2
    sprocket[sprocket < threshold] = 0
    sprocket[sprocket >= threshold] = 255
    sprocketSlice = sprocket[:,50:200]
    if options.debug and sprockets_dir is not None:
        scipy.misc.imsave('%s/%s/sprocket_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)), sprocket)
        scipy.misc.imsave('%s/%s/slice_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)), sprocketSlice)

    rangeDict = {}
    for line in zip(*sprocketSlice[::-1]):
        range = whitePixels(reversed(line))
        if 0 == len(range): continue
        range = range[0]
        rangeDict[range[1]] = range[0]

    useRange = []
    useRange.append(rangeDict[sorted(rangeDict.keys())[-1]])
    useRange.append(sorted(rangeDict.keys())[-1])

    sprocketCx = int(270 - SprocketSuper8.h/2)
    sprocketCy = int(useRange[0] + SprocketSuper8.w/2)
    (xAdj, yAdj, wAdj, hAdj) = (0, 0, 0, 0)
    if options.adjfile:
        try:
            (xAdj, yAdj, wAdj, hAdj) = [int(xx) for xx in open(options.adjfile).read().rstrip().split(',')]
            logger.debug("xAdj %d yAdj %d wAdj %d hAdj %d" % (xAdj, yAdj, wAdj, hAdj))
        except:
            logger.debug("%s file not found", options.adjfile)

    frameX = int(sprocketCx + SprocketSuper8.h/2) + xAdj
    frameY = int(sprocketCy - FrameSuper8.h/2) + yAdj 
    frameH = FrameSuper8.h + hAdj
    frameW = FrameSuper8.w + wAdj

    if options.debug and eroded_dir is not None:
        idraw = ImageDraw.Draw(sPlacement)
        line1 = (sprocketCx - SprocketSuper8.w/2, sprocketCy - SprocketSuper8.h/2,
                 sprocketCx + SprocketSuper8.w/2, sprocketCy + SprocketSuper8.h/2)
            
        line2 = (sprocketCx + SprocketSuper8.w/2, sprocketCy - SprocketSuper8.h/2,
                 sprocketCx - SprocketSuper8.w/2, sprocketCy + SprocketSuper8.h/2)
            
        idraw.line(line1, fill=255, width=2)
        idraw.line(line2, fill=255, width=2)
        idraw.line([i + j for i, j in zip(line1, (0, 1, 0, 1))], fill=0, width=2)
        idraw.line([i + j for i, j in zip(line2, (0, 1, 0, 1))], fill=0, width=2)

        # top horizontal
        fline = (frameX, frameY, frameX + frameW, frameY)
        idraw.line(fline, fill=255, width=2)
        idraw.line([i + j for i, j in zip(fline, (0, 1, 0, 1))], fill=0, width=2)

        # bottom horizontal
        fline = (frameX, frameY + frameH, frameX + frameW, frameY + frameH)
        idraw.line(fline, fill=255, width=2)
        idraw.line([i + j for i, j in zip(fline, (0, 1, 0, 1))], fill=0, width=2)

        # left vertical
        fline = (frameX, frameY, frameX, frameY + frameH)
        idraw.line(fline, fill=255, width=2)
        idraw.line([i + j for i, j in zip(fline, (1, 0, 1, 0))], fill=0, width=2)

        # right vertical
        fline = (frameX + frameW, frameY, frameX + frameW, frameY + frameH)
        idraw.line(fline, fill=255, width=2)
        idraw.line([i + j for i, j in zip(fline, (1, 0, 1, 0))], fill=0, width=2)

        sPlacement.save('%s/%s/2_%s' % (options.outputdir, eroded_dir, os.path.basename(filename)))
    # crop and save
    if ((frameX + frameW) > fcWidth) or ((frameY + frameH) > fcHeight):
        logger.error("Crop tile out of bounds %u x %u > %u x %u" % (frameX + frameW, fcWidth, frameY + frameH, fcHeight))
        sys.exit(1)

    for iFile in files:
        try:
            fullColor = PILImage.open(iFile)
            fullColor = fullColor.crop((int(frameX), int(frameY),
                 int(frameX + frameW),
                 int(frameY + frameH)))
            fullColor.save('%s/%s' % (outputpath, os.path.basename(iFile)))
        except:
            logger.error("Did not save %s/%s" % (outputpath, os.path.basename(iFile)))
            raise

def writeThresholds(sprocket):
    ss = copy.deepcopy(sprocket)
    for ii in range(0, 255):
        sprocket[sprocket < ii] = 0
        sprocket[sprocket >= ii] = 255
        if options.debug and sprockets_dir is not None:
            scipy.misc.imsave('%s/%s/sprocket_%s.jpg' % (options.outputdir, sprockets_dir, ii), sprocket)
        sprocket = copy.deepcopy(ss)

def maxRect(histogram):
    """Find height, width of the largest rectangle that fits entirely under
    the histogram."""
    stack = []
    top = lambda: stack[-1]
    max_size = (0, 0) # height, width of the largest rectangle
    pos = 0 # current position in the histogram
    endCol = 0
    for pos, height in enumerate(histogram):
        start = pos # position where rectangle starts
        while True:
            if not stack or height > top().height:
                stack.append(Info(start, height)) # push
            elif stack and height < top().height:
                thisArea = (top().height, pos - top().start)
                if (thisArea[0] * thisArea[1]) > (max_size[0] * max_size[1]):
                    endCol = pos
                    max_size = thisArea
                start, _ = stack.pop()
                continue
            break # height == top().height goes here

    pos += 1
    for start, height in stack:
	    if (max_size[0] * max_size[1]) < (height * (pos - start)):
		    max_size = (height, (pos - start))
        #max_size = max(max_size, (height, (pos - start)), key=area)    
#    if 0 == endCol: endCol = len(histogram)
    return max_size, endCol - 1

#def area(size):
#    return reduce(mul, size)

def findLargestRect(mat, value=0):
    """Find height, width of the largest rectangle containing all `value`'s."""
    it = iter(mat)
    rowIdx = 1
    endRow = 0
    endCol = 0
    hist = [(el==value) for el in next(it, [])]
    max_size, endCol = maxRect(hist)
    for row in it:
        hist = [(1+h) if el == value else 0 for h, el in zip(hist, row)]
        this_size, thisEnd = maxRect(hist)
        #print "row %u this_size %s thisEnd %u" % (rowIdx, this_size, thisEnd)
        if (this_size[0] * this_size[1]) > (max_size[0] * max_size[1]):
            max_size = this_size
            endRow = rowIdx
            endCol = thisEnd
        rowIdx += 1
    return max_size, (endRow, endCol)

def findExtents(tag, data):
    (nrows, ncols) = data.shape
    filename = "/tmp/%s%s.bin" % (tag, os.getpid())
    arrfile = open(filename,"wb")
    arrfile.write(struct.pack("II", *data.shape))
    data.tofile(arrfile)
    arrfile.close()
    runargs = ['/media/sf_vproj/scans/software/a.out', '-debug', filename]
    logger.debug("Calling %s" % ' '.join(runargs))
    extents = subprocess.check_output(runargs).decode(sys.stdout.encoding).strip().split(' ')
    os.remove(filename)
    return dict(zip(['top','bottom','right'],[int(xx) for xx in extents]))

def findExtents2(tag, data):
    (nrows, ncols) = data.shape
    arrfile = open("/tmp/%s%s.bin" % (tag, os.getpid()),"wb")
    arrfile.write(struct.pack("II", *data.shape))
    data.tofile(arrfile)
    arrfile.close()
    w = numpy.zeros(dtype=numpy.uint8, shape=data.shape)
    h = numpy.zeros(dtype=numpy.uint8, shape=data.shape)
    area_max = (0,[])
    for r in range(nrows):
        for c in range(ncols):
            if data[r][c] == 0:
                continue
            if r == 0:
                h[r][c] = 1
            else:
                h[r][c] = h[r-1][c]+1
            if c == 0:
                w[r][c] = 1
            else:
                w[r][c] = w[r][c-1]+1
            minw = w[r][c]
            for dh in range(h[r][c]):
                minw = min(minw, w[r-dh][c])
                area = (dh+1)*minw
                if area > area_max[0]:
                    area_max = (area, [(r-dh, c-minw+1, r, c)])
    return area_max

#    pdb.set_trace()
#    print('area', area_max[0])
#    for t in area_max[1]:
#        print('Cell 1:({}, {}) and Cell 2:({}, {})'.format(*t))
#    pass
    
#    for line in zip(*data[::-1]):
#        range = whitePixels(reversed(line))
#        if 0 == len(range): continue
#        range = range[0]
#        rangeDict[range[1]] = range[0]
    #rect = findLargestRect(data, 1)

    # find largest rectangle
    # find y center of triangle
    # exclude groups that dont encompass y center
    # find average height
    # exclude <10% and >90%

def process8mm(filenames, outputpath):
    files = filenames.split(',')
    if options.single:
        filename = files[0]
    else:
        if 3 != len(files):
            logger.error("Need three filenames")
            sys.exit(1)
        filename = files[1]

    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    if options.debug and eroded_dir is not None:
        sPlacement = imp
    (fcHeight, fcWidth) = flattened.shape
    sprocket = ndimage.grey_erosion(flattened[:,:300], size=(25, 25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(sprocket)
    lightest = ndimage.maximum(sprocket)

#    writeThresholds(sprocket)

    threshold = int(darkest + (lightest - darkest)/2)
    logger.debug("Threshold is {}".format(threshold))
    sprocket[sprocket < threshold] = 0
    sprocket[sprocket >= threshold] = 255
    sprocketSlice = sprocket[:,:100]
    if options.debug and sprockets_dir is not None:
        scipy.misc.imsave('%s/%s/sprocket_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)), sprocket)
        scipy.misc.imsave('%s/%s/slice_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)), sprocketSlice)

#    rangeDict = {}

    #for line in zip(*sprocketSlice[::-1]):
    #    range = whitePixels(reversed(line))
    #    if 0 == len(range): continue
    #    range = range[0]
    #    rangeDict[range[1]] = range[0]
    #    '157', '469', '88'] top,bottom,right
    upperSprocket = findExtents("upper_%s" % os.path.basename(filename), sprocketSlice[:550])
    lowerSprocket = findExtents("lower_%s" % os.path.basename(filename), sprocketSlice[1200:])
    #(Pdb) upperSprocket
    #(24735, [(188, 0, 442, 96)]) y,x,y,x
    #(Pdb) lowerSprocket
    #(13360, [(211, 0, 377, 79)]) y,x,y,x

    frameCy = int(upperSprocket['bottom'] + (lowerSprocket['top'] + 1200 - upperSprocket['bottom'])/2)

    (xAdj, yAdj, wAdj, hAdj) = (0, 0, 0, 0)
    if options.adjfile:
        try:
            (xAdj, yAdj, wAdj, hAdj) = [int(xx) for xx in open(options.adjfile).read().rstrip().split(',')]
            logger.debug("xAdj %d yAdj %d wAdj %d hAdj %d" % (xAdj, yAdj, wAdj, hAdj))
        except:
            logger.debug("%s file not found", options.adjfile)
    frameX = upperSprocket['right'] + xAdj
    frameY = frameCy - Frame8mm.h/2 + yAdj
    frameH = Frame8mm.h + hAdj
    frameW = Frame8mm.w + wAdj


    if options.debug and sprockets_dir is not None:
        idraw = ImageDraw.Draw(sPlacement)
        # upper sprocket
        xy = numpy.array([0, upperSprocket['top'], upperSprocket['right'], upperSprocket['bottom']])
        idraw.rectangle([*xy])
        # lower sprocket
        xy = numpy.array([0, lowerSprocket['top'], lowerSprocket['right'], lowerSprocket['bottom']]) + numpy.array([0,1200,0,1200])
#        xy = numpy.array(lowerSprocket)[[1,0,3,2]] + numpy.array([0,1200,0,1200])
        idraw.rectangle([*xy])
        # center line
        idraw.line([0, frameCy-1, fcWidth, frameCy-1], fill=255,width=1)
        idraw.line([0, frameCy, fcWidth, frameCy], fill=0,width=1)
        idraw.line([0, frameCy+1, fcWidth, frameCy+1], fill=255,width=1)
        # frame
        idraw.rectangle([frameX-1, frameY-1, frameX + frameW-1, frameY + frameH-1],  outline=255)
        idraw.rectangle([frameX, frameY, frameX + frameW, frameY + frameH], outline=0)
        idraw.rectangle([frameX+1, frameY + 1, frameX + frameW+1, frameY + frameH+1], outline=255)
        sPlacement.save('%s/%s/rect_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)))

    # crop and save
    if ((frameX + frameW) > fcWidth) or ((frameY + frameH) > fcHeight):
        logger.error("Crop tile out of bounds %u x %u > %u x %u" % (frameX + frameW, frameY + frameH, fcWidth, fcHeight))
        sys.exit(1)

    if options.single:
        files = [files[0]]
    for iFile in files:
        try:
            fullColor = PILImage.open(iFile)
            fullColor = fullColor.crop((int(frameX), int(frameY),
                 int(frameX + frameW),
                 int(frameY + frameH)))
            fullColor.save('%s/%s' % (outputpath, os.path.basename(iFile)))
        except:
            logger.error("Did not save %s/%s" % (outputpath, os.path.basename(iFile)))
            raise

def main():
    if options.mode not in ('8mm', 'super8'):
        logger.error("Invalid sprocket option %s" % options.sprocket)
        sys.exit(1)

    processor = {'super8': processSuper8, '8mm': process8mm }[options.mode]

    if options.filenames is not None:
        processor(options.filenames, options.outputdir)
        return

    if options.listfile is not None:
        triples = open(options.listfile, "r").readlines()
        for triple in triples:
            processor(triple.strip(), options.outputdir)
        return

    files = sorted(glob('%s/*.JPG' % options.inputdir))
    for ii in range(0, len(files) - 3, 3):
        triple = ','.join(files[ii:ii+3])
        if options.debug:
            logger.debug("Autocropping %s" % triple)
        processor(triple, options.outputdir)

main()

