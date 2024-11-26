#!/usr/bin/python3
# autocrop for rotated images

import scipy
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

ImageFile.LOAD_TRUNCATED_IMAGES = True
XFudge = 225

options = {}

parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')
parser.add_option('-s', '--debug', dest='debug', help='Save intermediary images', action='store_true')
parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False)
parser.add_option('-w', '--whitecount', dest='whitecount', action='store_true', default=False)
parser.add_option('-i', '--input-dir', dest='inputdir')
parser.add_option('-o', '--output-dir', dest='outputdir')
parser.add_option('-f','--filenames', dest='filenames', help='Three filenames, comma separated')
parser.add_option('-m','--mode', dest='mode', default='8mm')
parser.add_option('-l','--listfile', dest='listfile', help='file containing list of triplets')
parser.add_option('-a','--adjfile', dest='adjfile', help='frame adjustmentfile (x,y,w,h)')

(options, args) = parser.parse_args()


#greyed_dir = {True:'greyed', False: None}[os.path.isdir('%s/greyed' % options.outputdir)]
#bw_dir = {True:'bw', False: None}[os.path.isdir('%s/bw' % options.outputdir)]
#corner_dir = {True:'corner', False: None}[os.path.isdir('%s/corner' % options.outputdir)]
sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('%s/sprockets' % options.outputdir)]
eroded_dir = {True:'eroded', False: None}[os.path.isdir('%s/eroded' % options.outputdir)]

#arrToFind = None

#FrameHeightMm = 3.3
#FrameWidthMm = 4.5
#SprocketSuper8 = (1.14, .91) # H, W
#Sprocket8mm = (1.27, 1.83) # H, W
PxPerMm8mm = 391
PxPerMmSuper8 = 360
Info = namedtuple('Info', 'start height')
# 
def roundIt(val): return int(val + val%2)
SprocketSuper8 = type('obj', (object,), {'h': roundIt(1.14 * PxPerMmSuper8),
    'w': roundIt(.91 * PxPerMmSuper8)})
FrameSuper8 = type('obj', (object,), {'h': roundIt(5.46 * PxPerMmSuper8),
    'w': roundIt(4.01 * PxPerMmSuper8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'h': roundIt(4.37 * PxPerMm8mm),
    'w': roundIt(3.28 * PxPerMm8mm)})

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(message)s')
logger = logging.getLogger('autocrop')
#fileHandler = RotatingFileHandler(filename='/tmp/autocrop_%u.log' % os.getpid(), maxBytes=10e6, backupCount=2)
#fileHandler.setLevel(logging.DEBUG)
#logger.addHandler(fileHandler)

#def whiteCount(filename):
#    imp = PILImage.open(filename).convert('L')
#    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
#    (height, width) = im.shape
#
#    im[im < 100] = 0
#    im[im >= 100] = 255
#
#    whiteCount = numpy.count_nonzero(im)
#    index = whiteCount / (height * width)
#
#    print "%s: Total %u white %u index %f" % (filename, height * width, whiteCount, index)

def whitePixels(data, minlen, maxlen):
    whites = []

    idx = 0
    for kk, gg in groupby(data, lambda x: x):
        count = sum(1 for _ in gg)
        #if 1 == kk:
        if 255 == kk:
            whites.append([idx, count])
        idx += count
    return whites

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
        if area(this_size) > area(max_size):
            max_size = this_size
            endRow = rowIdx
            endCol = thisEnd
        rowIdx += 1
    return max_size, (endRow, endCol)

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
                if area(thisArea) > area(max_size):
                    endCol = pos
                    max_size = thisArea
                start, _ = stack.pop()
                continue
            break # height == top().height goes here

    pos += 1
    for start, height in stack:
        max_size = max(max_size, (height, (pos - start)), key=area)    
#    if 0 == endCol: endCol = len(histogram)
    return max_size, endCol - 1

def area(size):
    return reduce(mul, size)

def findSuper8Sprocket4(image):
    info = findLargestRect(image, 255)
    return info[1]

def findSuper8Sprocket3(image, filename):
    #arrToFind = scipy.ones((300,200), numpy.uint8)
    #arrToFind = scipy.ones((400,300), numpy.uint8)
    arrToFind = scipy.ones((100,100), numpy.uint8)
    matches = cv2m(image, arrToFind, cv2.TM_CCOEFF)
    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(matches)
    return maxLoc

def findSuper8Sprocket2(image, filename):
    (label, numFeatures) = scipy.ndimage.measurements.label(image)
    found = ndimage.find_objects(label)
    (imX, imY) = image.size

    closest = imY
    (nomX, nomY) = (None, None)
    for elem in found:
        (yY, xX) = elem
        if xX.stop - xX.start < 200 or yY.stop - yY.start < 300:
            continue
        distance = abs((imY / 2) - ((yY.stop + yY.start) / 2))
        if distance < closest:
            closest = distance
            (nomX, nomY) = (xX, yY)

    return (nomX.start, (nomY.stop + nomY.start) / 2)

def findSuper8Sprocket(image, filename):
    (imX, imY) = image.size
    fromLeft = {}
    fromRight = {}
    #leftExtent = {}
    imgData = list(image.getdata())
    # Scan each row from the left looking for white, store in fromLeft
    # Then continue to scan for black, store in leftExtent
    for ii in range(0, imX * imY - imX, imX):
        row = ii / imX
        try:
            fromLeft[row] = imgData[ii:ii + imX].index(255)
        except ValueError:
            pass
    for row in fromLeft.keys():
        try:
            ii = row * imX
            fromRight[row] = imgData[ii + imX - 1:ii:-1].index(0)
        except ValueError:
            pass

    # fromLeft and leftExtent contain data for rows that have both
    # black and white pixels

    if options.debug and sprockets_dir is not None:
        imaged = ImageDraw.Draw(image)
        for row, value in fromLeft.iteritems():
            imaged.line((0, row, value, row), fill=192)

        for row, value in fromRight.iteritems():
            offset = fromLeft[row]
            imaged.line((offset, row, offset + value, row), fill=96)

        logger.debug("Saved %s/%s" % ((sprockets_dir, os.path.basename(filename))))
        image.save('%s/%s/%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)))

    # candidateRows contain white areas that could be a sprocket
    candidateRows = {kk for kk, vv in fromRight.iteritems() if vv > 100}
    candidateRanges = []
    for kk, gg in groupby(enumerate(candidateRows), lambda i,x: i - x):
        group = map(itemgetter(1), gg)
        candidateRanges.append((group[0], group[-1]))

    logger.debug("%u candidate ranges" % len(candidateRanges))

    # candidateRanges is a list of candidate row ranges
    # look for the one that's closest to the middle
    nominatedRange = ()
    closest = imY
    for range in candidateRanges:
        distance = abs((imY / 2) - (sum(range) / 2))
        logger.debug("(%u,%u) distance %u closest %u" % (range[0], range[1], distance, closest))
        if distance < closest:
            closest = distance
            nominatedRange = range

    if 0 == len(nominatedRange):
        logger.error("No nominated range found!")
        return (0, 0)

    # return center of sprocket
    avgY = sum(nominatedRange) / 2
    return (fromLeft[avgY], avgY)
    #return (fromLeft[avgY] + (leftExtent[avgY]/2), avgY)

def processSuper8(filenames, outputpath):
    files = filenames.split(',')
    if 3 != len(files):
        logger.error("Need three filenames")
        sys.exit(1)

    # Select the brightest image for figuring out the cropping
    filename = files[0]
    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    if options.debug and eroded_dir is not None:
        sPlacement = imp
    (fcHeight, fcWidth) = flattened.shape
    xOffset = 600
    yOffset = fcHeight - 135
    #yOffset = 0 # fcHeight - 135
    flattened = flattened[yOffset:,:]
    eroded = ndimage.grey_erosion(flattened, size=(25, 25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(eroded)
    lightest = ndimage.maximum(eroded)

    ethreshold = darkest + (lightest - darkest)/2
    eroded[eroded < ethreshold] = 0
    eroded[eroded >= ethreshold] = 255
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('%s/%s/1_%s' % (options.outputdir, eroded_dir, os.path.basename(filename)), eroded)

    rangeDict = {}
    for range in whitePixels(eroded[-1], 200, 400):
        if eroded.shape[1] > (range[0] + 300):
            logger.debug('Candidate sprocket range %s' % range)
            rangeDict[abs(int(range[0]+(range[1]/2)) - (fcWidth/2))] = range

    useRange = rangeDict[sorted(rangeDict.keys())[0]]

    # find the horizontal extents of the sprocket
    # remove top and bottom 150
    lookFor = numpy.ones([135,SprocketSuper8.w], dtype=numpy.uint8)
    methods = [cv2.TM_CCOEFF,cv2.TM_CCOEFF_NORMED,cv2.TM_CCORR,cv2.TM_CCORR_NORMED,
        cv2.TM_SQDIFF,cv2.TM_SQDIFF_NORMED]

    res = cv2.matchTemplate(eroded[:,useRange[0]:sum(useRange)], lookFor, methods[2])
    (minval, maxval, minloc, maxloc) = cv2.minMaxLoc(res)

    sprocketCx = useRange[0] + maxloc[1] + (SprocketSuper8.w / 2)
    sprocketCy = fcHeight - 135 - 135 + maxloc[0] + (SprocketSuper8.h / 2)

    (xAdj, yAdj, wAdj, hAdj) = (0, 0, 0, 0)
    if options.adjfile:
        try:
            (xAdj, yAdj, wAdj, hAdj) = [int(xx) for xx in open(options.adjfile).read().rstrip().split(',')]
            logger.debug("xAdj %d yAdj %d wAdj %d hAdj %d" % (xAdj, yAdj, wAdj, hAdj))
        except:
            logger.debug("%s file not found", options.adjfile)

    frameX = (sprocketCx - FrameSuper8.w / 2) + xAdj
    frameY = sprocketCy - (SprocketSuper8.h / 2) - FrameSuper8.h + yAdj 
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

#    if options.debug and bw_dir is not None:
#        bwd = ImageDraw.Draw(imp)
#        bwd.line((0, spCenterY, 600, spCenterY), fill=0)
#        bwd.line((spRightX, spCenterY - (2.015 * PxPerMmSuper8),
#            spRightX, spCenterY + (2.015 * PxPerMmSuper8)), fill = 0)
#        bwd.rectangle((frameOriginX, frameOriginY,
#            frameOriginX + frameWidth,
#            frameOriginY + frameHeight), fill=192)
#        imp.save('%s/%s/%s' % (options.outputdir, bw_dir, os.path.basename(filename)))

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

def find8mmSprocket(image, filename):
    (imX, imY) = image.size
    imgData = list(image.getdata())
    fromLeft = {}
    fromRight = {}
    # Scan each row from the left looking for white, store in fromLeft
    # Then continue to scan for black, store in leftExtent
    for ii in range(0, imX * imY - imX, imX):
        row = ii / imX
        try:
            fromLeft[row] = imgData[ii:ii + imX].index(255)
        except ValueError:
            fromLeft[row] = imX

    for row in fromLeft.keys():
        try:
            ii = row * imX
            fromRight[row] = imgData[ii + imX - 1:ii:-1].index(0)
        except ValueError:
            pass

    if options.debug and sprockets_dir is not None:
        imaged = ImageDraw.Draw(image)
        for row, value in fromLeft.iteritems():
            imaged.line((0, row, value, row), fill=192)

        for row, value in fromRight.iteritems():
            offset = fromLeft[row]
            imaged.line((offset, row, offset + value, row), fill=96)

        logger.debug("Saved %s/%s" % ((sprockets_dir, os.path.basename(filename))))
        image.save('%s/%s/%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)))

    whiteRows = {kk for kk, vv in fromLeft.iteritems() if vv < (imX - 50)}
    whiteRanges = []
    for kk, gg in groupby(enumerate(whiteRows), lambda i,x: i - x):
        group = map(itemgetter(1), gg)
        whiteRanges.append((group[0], group[-1]))

    blackRows = {kk for kk, vv in fromLeft.iteritems() if vv > (imX - 50)}
    blackRanges = []
    for kk, gg in groupby(enumerate(blackRows), lambda i,x: i - x):
        group = map(itemgetter(1), gg)
        blackRanges.append((group[0], group[-1]))

    logger.debug("blackRanges %u whiteRanges %u" % (len(blackRanges), len(whiteRanges)))

    nominatedRange = ()
    closest = imY
    for range in blackRanges:
        distance = abs((imY / 2) - (sum(range) / 2))
        logger.debug("(%u,%u) distance %u closest %u" % (range[0], range[1], distance, closest))
        if distance < closest:
            closest = distance
            nominatedRange = range

    if 0 == len(nominatedRange):
        logger.error("No nominated range found!")
        return (0, 0)

    # return center of sprocket
    avgY = sum(nominatedRange) / 2
    return (150, avgY)
    #return (fromLeft[avgY] + (leftExtent[avgY]/2), avgY)
#    return (0, 0)
'''    numCandidates = len(candidateRanges)
    if numCandidates < 2:
        logger.error("Only %u candidate ranges, cannot process" % numCandidates)
        return(0, 0)

    #nominatedRange = ()
    closest = imY
    topRow = 0
    bottomRow = 0
    leftExtentRow = 0
    for ii in range(0, numCandidates):
        for jj in range(0, numCandidates):
            if ii == jj:
                continue
            avg1 = sum(candidateRanges[ii]) / 2
            avg2 = sum(candidateRanges[jj]) / 2
            distance = (avg1 + avg2) / 2
            logger.debug("avg1 %u avg2 %u distance %u" % (avg1, avg2, distance))

            if distance < closest:
                closest = distance
                topRow = candidateRanges[ii][-1]
                bottomRow = candidateRanges[jj][0]
                leftExtentRow = sum(candidateRanges[ii]) / 2
                logger.debug("New top row %u bottom %u" % (topRow, bottomRow))

#    for range in candidateRanges[:-1]:
#        distance = abs((imY / 2) - (sum(range) / 2))
#        if options.debug:
#            logger.debug("(%u,%u) distance %u closest %u" % (range[0], range[1], distance, closest))
#        if distance < closest:
#            closest = distance
#            nominatedRange = range

#    if 0 == len(nominatedRange):
#        if options.debug:
#            logger.error("No nominated range found!")
#        return (0, 0)

    # return center of sprocket
    avgY = (topRow + bottomRow) / 2
    return (fromLeft[leftExtentRow], avgY)
    '''

def process8mm(filenames, outputpath):
    files = filenames.split(',')
    if 3 != len(files):
        logger.error("Need three filenames")
        sys.exit(1)

    erodedHeight = 550
    filename = files[0]
    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    if options.debug and eroded_dir is not None:
        sPlacement = imp
    (fcHeight, fcWidth) = flattened.shape
    xOffset = 600
    yOffset = fcHeight - erodedHeight
    flattened = flattened[yOffset:,:]
    eroded = ndimage.grey_erosion(flattened, size=(25, 25))

    eroded[eroded < 128] = 0
    eroded[eroded >= 128] = 1 
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('%s/%s/1_%s' % (options.outputdir, eroded_dir, os.path.basename(filename)), eroded)

    rangeDict = {}
    for range in whitePixels(eroded[-1], 200, 400):
        if eroded.shape[1] > (range[0] + 300):
            rangeDict[abs(int(numpy.mean(range)) - (fcWidth/2))] = range

    useRanges = [rangeDict[key] for key in sorted(rangeDict.keys())[:2]]
    useRange = [(useRanges[0][0] + useRanges[1][0])/2, (useRanges[0][1] + useRanges[1][1])/2 ]

    # find the horizontal extents of the sprocket
    # remove top and bottom 150
    lookFor = numpy.ones([erodedHeight,Sprocket8mm.w], dtype=numpy.uint8)
    methods = [cv2.TM_CCOEFF,cv2.TM_CCOEFF_NORMED,cv2.TM_CCORR,cv2.TM_CCORR_NORMED,
        cv2.TM_SQDIFF,cv2.TM_SQDIFF_NORMED]

    #res = cv2.matchTemplate(eroded[:,useRanges[0][0]:sum(useRanges[0])], lookFor, methods[2])
    res = cv2.matchTemplate(eroded[:,useRanges[1][0]:sum(useRanges[1])], lookFor, methods[2])
    (minval, maxval, minloc, maxloc) = cv2.minMaxLoc(res)

    sprocketCx = (sum(useRanges[0]) + useRanges[1][0])/2 # (Sprocket8mm.w / 2)
    sprocketCy = fcHeight - erodedHeight +  maxloc[0] + (Sprocket8mm.h / 2)

    (xAdj, yAdj, wAdj, hAdj) = (0, 0, 0, 0)
    if options.adjfile:
        try:
            (xAdj, yAdj, wAdj, hAdj) = [int(xx) for xx in open(options.adjfile).read().rstrip().split(',')]
        except:
            logger.debug("%s file not found", options.adjfile)
            pass

    logger.debug("xAdj %d yAdj %d wAdj %d hAdj %d" % (xAdj, yAdj, wAdj, hAdj))

    frameX = (sprocketCx - Frame8mm.w / 2) + xAdj
    frameY = sprocketCy - (Sprocket8mm.h / 2) - Frame8mm.h + yAdj 

    frameH = Frame8mm.h + hAdj
    frameW = Frame8mm.w + wAdj

    if options.debug and eroded_dir is not None:
        idraw = ImageDraw.Draw(sPlacement)
        line1 = (sprocketCx - Sprocket8mm.w/2, sprocketCy - Sprocket8mm.h/2,
                 sprocketCx + Sprocket8mm.w/2, sprocketCy + Sprocket8mm.h/2)
            
        line2 = (sprocketCx + Sprocket8mm.w/2, sprocketCy - Sprocket8mm.h/2,
                 sprocketCx - Sprocket8mm.w/2, sprocketCy + Sprocket8mm.h/2)
            
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

def main():
    if options.mode not in ('8mm', 'super8'):
        logger.error("Invalid sprocket option %s" % options.sprocket)
        sys.exit(1)

#    imgToFind = PILImage.open('%s/tofind.jpg' % os.path.dirname(os.path.abspath(__file__))).convert('L')
#    arrToFind = scipy.misc.fromimage(imgToFind, flatten = True).astype(numpy.uint8)
    #arrToFind = arrToFind[:300,:220]
#    arrToFind = arrToFind[:50,:50]

    #arrFindIn[arrFindIn < 100] = 0
    #arrFindIn[arrFindIn >= 100] = 255

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

