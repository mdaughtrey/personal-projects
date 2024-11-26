#!/usr/bin/python

import scipy
from scipy import ndimage
from scipy import signal
from optparse import OptionParser
from PIL import Image as PILImage, ImageDraw, ImageFilter, ImageFile
import Image
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
from cv2 import matchTemplate as cv2m
from collections import namedtuple
from operator import mul

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

(options, args) = parser.parse_args()


#greyed_dir = {True:'greyed', False: None}[os.path.isdir('%s/greyed' % options.outputdir)]
#bw_dir = {True:'bw', False: None}[os.path.isdir('%s/bw' % options.outputdir)]
#corner_dir = {True:'corner', False: None}[os.path.isdir('%s/corner' % options.outputdir)]
sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('%s/sprockets' % options.outputdir)]
eroded_dir = {True:'eroded', False: None}[os.path.isdir('%s/eroded' % options.outputdir)]

#arrToFind = None

FrameHeightMm = 3.3
FrameWidthMm = 4.5
SprocketSuper8 = (1.14, .91) # H, W
Sprocket8mm = (1.27, 1.83) # H, W
PxPerMm8mm = 454
PxPerMmSuper8 = 352
Info = namedtuple('Info', 'start height')

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
    for ii in xrange(0, imX * imY - imX, imX):
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
    for kk, gg in groupby(enumerate(candidateRows), lambda (i,x): i - x):
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
    filename = files[1]
    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    if options.debug and eroded_dir is not None:
        sPlacement = imp
    #(fcWidth, fcHeight) = im.shape
    (fcHeight, fcWidth) = flattened.shape
    xOffset = 600
    yOffset = fcHeight - 400
    # heightFrom:heightTo,xFrom:xTo
    # flattened = bottom 600, clipped left and right by 600
    flattened = flattened[yOffset:,xOffset:-600]
    eroded = ndimage.grey_erosion(flattened, size=(25, 25))

    eroded[eroded < 128] = 0
    eroded[eroded >= 128] = 255
    eroded[:,-1] = 0
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('%s/%s/1_%s' % (options.outputdir, eroded_dir, os.path.basename(filename)), eroded)

    # find the horizontal extents of the sprocket
    # remove top and bottom 150
    ((_, width), (_, rightx)) = findLargestRect(eroded[260:,:], 255)
    sprocketX = xOffset + rightx - width
    sprocketW = width

    # show the bottom 600 and around the extens
    ((height, _), (bottomY, _)) = findLargestRect(eroded[:,rightx - width - 20:rightx + 20], 255)

    sprocketH = height
    sprocketY = bottomY - height + yOffset
    sprocketCx = sprocketX + sprocketW / 2
    sprocketCy = sprocketY + sprocketH / 2

    frameW = int((4.01 + 0.6) * PxPerMmSuper8)
    if frameW % 2 == 1:
        frameW += 1
    frameH = int((5.79 + 0.2) * PxPerMmSuper8)
    if frameH % 2 == 1:
        frameH += 1

    frameX = (sprocketCx - frameW / 2) + XFudge
    frameY = sprocketCy - (sprocketH / 2) - frameH

    frameY += 50
    frameX -= 120
    frameH -= 70
# adjust
    frameW -= 100

    if options.debug and eroded_dir is not None:
        idraw = ImageDraw.Draw(sPlacement)

        line1 = (sprocketX, sprocketY, sprocketX + sprocketW, sprocketY + sprocketH)
        line2 = (sprocketX, sprocketY + sprocketH, sprocketX + sprocketW, sprocketY)
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
    for ii in xrange(0, imX * imY - imX, imX):
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
    for kk, gg in groupby(enumerate(whiteRows), lambda (i,x): i - x):
        group = map(itemgetter(1), gg)
        whiteRanges.append((group[0], group[-1]))

    blackRows = {kk for kk, vv in fromLeft.iteritems() if vv > (imX - 50)}
    blackRanges = []
    for kk, gg in groupby(enumerate(blackRows), lambda (i,x): i - x):
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
    for ii in xrange(0, numCandidates):
        for jj in xrange(0, numCandidates):
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

    # Select the brightest image for figuring out the cropping
    filename = files[0]
    imp = PILImage.open(filename).convert('L')
    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    #(fcWidth, fcHeight) = im.shape
    (fcHeight, fcWidth) = flattened.shape
    #flattened = flattened[:800,:800]
    flattened = flattened[:1200,:1200]
    eroded = ndimage.grey_erosion(flattened, size=(25, 25))

    eroded[eroded < 200] = 0
    eroded[eroded >= 200] = 255
    eroded[:,-1] = 0
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('%s/%s/%s' % (options.outputdir, eroded_dir, os.path.basename(filename)), eroded)

# super8 sprocket size 0.91W 1.14H centered
# 8mm sprocket size 1.8W 1.23H between
#    pdb.set_trace()
    ((rectH, rectW), (spBottomY, spRightX)) = findLargestRect(eroded, 255)
    
    spCenterY = int(((spBottomY - rectH / 2)) + (3.3 * PxPerMm8mm / 2)) #  + 700) # * PxPerMmSuper8) + 700
    logger.debug("%s spRightX %u spCenterY %u" % (filename, spRightX, spCenterY))

    frameWidth = int(4.5 * PxPerMm8mm)
    if frameWidth % 2 == 1:
        frameWidth += 1
    frameHeight = int(3.3 * PxPerMm8mm)

    frameOriginX = int(spRightX) # int((spRightX - rectW) + ((0.91 + .05) * PxPerMm8mm))
    frameOriginY = int(spCenterY - frameHeight/ 2)

    logger.debug("frame X %u Y %u W %u H %u" % (frameOriginX, frameOriginY, frameWidth, frameHeight))

    # crop and save
    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
        logger.error("Crop tile out of bounds %u x %u > %u x %u" % (frameOriginX + frameWidth, fcWidth, frameOriginY + frameHeight, fcHeight))
        sys.exit(1)

#    if options.debug and bw_dir is not None:
#        bwd = ImageDraw.Draw(imp)
#        bwd.line((0, spCenterY, 600, spCenterY), fill=0)
#        bwd.line((spRightX, spCenterY - (1.62 * PxPerMm8mm),
#            spRightX, spCenterY + (1.62 * PxPerMm8mm)), fill = 0)
#        bwd.rectangle((frameOriginX, frameOriginY,
#            frameOriginX + frameWidth,
#            frameOriginY + frameHeight), fill=192)
#        imp.save('%s/%s/%s' % (options.outputdir, bw_dir, os.path.basename(filename)))

    for iFile in files:
        try:
            fullColor = PILImage.open(iFile)
            fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
                 int(frameOriginX + frameWidth),
                 int(frameOriginY + frameHeight)))
            fullColor.save('%s/%s' % (outputpath, os.path.basename(iFile)))
        except:
            logger.error("Did not save %s/%s" % (outputpath, os.path.basename(iFile)))

def main():
    if options.mode not in ('8mm', 'super8'):
        logger.error("Invalid sprocket option %s" % options.sprocket)
        sys.exit(1)

    imgToFind = PILImage.open('%s/tofind.jpg' % os.path.dirname(os.path.abspath(__file__))).convert('L')
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
    for ii in xrange(0, len(files) - 3, 3):
        triple = ','.join(files[ii:ii+3])
        if options.debug:
            logger.debug("Autocropping %s" % triple)
        processor(triple, options.outputdir)

main()

