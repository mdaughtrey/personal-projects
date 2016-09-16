#!/usr/bin/python

import scipy
from scipy import ndimage
from scipy import signal
from optparse import OptionParser
from PIL import Image as PILImage, ImageDraw, ImageFilter
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

greyed_dir = {True:'greyed', False: None}[os.path.isdir('greyed')]
bw_dir = {True:'bw', False: None}[os.path.isdir('bw')]
corner_dir = {True:'corner', False: None}[os.path.isdir('corner')]
sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('sprockets')]
eroded_dir = {True:'eroded', False: None}[os.path.isdir('eroded')]

options = {}
arrToFind = None

FrameHeightMm = 3.3
FrameWidthMm = 4.5
SprocketSuper8 = (1.14, .91) # H, W
Sprocket8mm = (1.27, 1.83) # H, W
#PxPerMm8mm = 380
#PxPerMmSuper8 = 393

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(message)s')
logger = logging.getLogger('autocrop')
fileHandler = RotatingFileHandler(filename='/tmp/autocrop_%u.log' % os.getpid(), maxBytes=10e6, backupCount=2)
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

def whiteCount(filename):
    imp = PILImage.open(filename).convert('L')
    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    (height, width) = im.shape

    im[im < 100] = 0
    im[im >= 100] = 255

    whiteCount = numpy.count_nonzero(im)
    index = whiteCount / (height * width)

    print "%s: Total %u white %u index %f" % (filename, height * width, whiteCount, index)

def findSuper8Sprocket3(image, filename):
    matches = cv2m(image, arrToFind, cv2.TM_SQDIFF)
    (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(matches)
    return minLoc

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
        image.save('%s/%s' % (sprockets_dir, os.path.basename(filename)))

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
    # Select the midrange image for figuring out the cropping
    filename = files[1]
    if 3 != len(files):
        logger.error("Need three filenames")
        sys.exit(1)

    if options.whitecount:
        for file in filenames:
            whiteCount(file)

    ofiles = [os.path.isfile("%s/%s" % (outputpath, os.path.basename(xx))) for xx in files]
    if [True, True, True] == ofiles:
        logger.debug("Output files aready exist for %s" % filenames)
        return

    imp = PILImage.open(filename).convert('L')
    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    #(fcWidth, fcHeight) = im.shape
    (fcHeight, fcWidth) = im.shape
    #im = im[:,:400]
    im = im[:,:600]
    im1 = ndimage.grey_erosion(im, size=(25, 25))

    im1[im1 < 100] = 0
    im1[im1 >= 100] = 255
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('eroded/%s' % os.path.basename(filename), im1)

    pxPerMm = 369
    im1Image = scipy.misc.toimage(im1)
    #(spLeftX, spCenterY) = findSuper8Sprocket3(im1Image, filename)
    (spLeftX, spCenterY) = findSuper8Sprocket3(im1, filename)
    #pdb.set_trace()
    spCenterY += (SprocketSuper8[0] * pxPerMm / 2 )
    logger.debug( "%s leftX %u centerY %u" % (filename, spLeftX, spCenterY))
    if (0, 0) == (spLeftX, spCenterY):
        logger.error("Cannot process %s" % filename)
        return

    frameOriginX = int(spLeftX + ((.8 + .1) * pxPerMm))
    frameOriginY = int(spCenterY - (2.015 * pxPerMm))

    # FUDGE FACTOR for misaligned images
    #frameOriginY -= (122  + (.455 * pxPerMm))
    frameOriginY -= ((.465 * pxPerMm))

    frameWidth = int(5.7 * pxPerMm)
    frameHeight = int(4.11 * pxPerMm)

    if frameWidth % 2 == 1:
        frameWidth += 1
    # crop and save
    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
        logger.error("Crop tile out of bounds %u x %u > %u x %u" % (frameOriginX + frameWidth, fcWidth, frameOriginY + frameHeight, fcHeight))
        return

    if options.debug and bw_dir is not None:
        bwd = ImageDraw.Draw(imp)
        bwd.line((spLeftX, spCenterY, fcWidth, spCenterY), fill=0)
        bwd.line((spLeftX, spCenterY - (2.015 * pxPerMm),
            spLeftX, spCenterY + (2.015 * pxPerMm)), fill = 0)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        imp.save('%s/%s' % (bw_dir, os.path.basename(filename)))

    for iFile in files:
        try:
            fullColor = PILImage.open(iFile)
            fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
                 int(frameOriginX + frameWidth),
                 int(frameOriginY + frameHeight)))
            fullColor.save('%s/%s' % (outputpath, os.path.basename(iFile)))
        except:
            logger.error("Did not save %s/%s" % (outputpath, os.path.basename(iFile)))

#    print "%s -> %s" % (filenames, outputpath)
#    for file in files:
#        try:
#            frame = PIL.Image.open(file)
#            frame = frame.crop((int(frameOriginX), int(frameOriginY),
#                int(frameOriginX + frameWidth),
#                int(frameOriginY + frameHeight)))
#            frame.save('%s/%s' % (outputpath, os.path.basename(file)))
#        except:
#            print "Did not save %s/%s" % (outputpath, os.path.basename(file))
#

#-    (top, bottom) = (0, 0)
#-    (sfWidth, sfHeight) = image.size
#-    midway = sfWidth * sfHeight / 2
#-    sfSequence = list(image.getdata())
#-
#-    compTo = []
#-    for jj in range(0, sfWidth):
#-        compTo.append(0)
#-
#-    for top in xrange(midway, 0 , -sfWidth):
#-        if sfSequence[top:top + sfWidth].count(0) > sfWidth / 2:
#-            break
#-#        if compTo == sfSequence[top:top + sfWidth]:
#-
#-    for bottom in xrange(midway, sfHeight * sfWidth, sfWidth):
#-        if sfSequence[bottom:bottom + sfWidth].count(0) > sfWidth / 2:
#-            break;
#-#        if compTo == sfSequence[bottom:bottom + sfWidth]:
#-
#-

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
        image.save('%s/%s' % (sprockets_dir, os.path.basename(filename)))

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
    # Select the midrange image for figuring out the cropping
    filename = files[1]
    if 3 != len(files):
        logger.error("Need three filenames")
        sys.exit(1)

    ofiles = [os.path.isfile("%s/%s" % (outputpath, os.path.basename(xx))) for xx in files]
    if [True, True, True] == ofiles:
        logger.debug("Output files aready exist for %s" % filenames)
        return

    if options.whitecount:
        for file in filenames:
            whiteCount(file)

    imp = PILImage.open(filename).convert('L')
    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    #(fcWidth, fcHeight) = im.shape
    (fcHeight, fcWidth) = im.shape
    #im = im[:,:400]
    im = im[:,:300]
    im1 = ndimage.grey_erosion(im, size=(25, 25))

    im1[im1 < 100] = 0
    im1[im1 >= 100] = 255
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('eroded/%s' % os.path.basename(filename), im1)

    im1Image = scipy.misc.toimage(im1)
    (spLeftX, spCenterY) = find8mmSprocket(im1Image, filename)
    spLeftX = 152
    logger.debug( "%s leftX %u centerY %u" % (filename, spLeftX, spCenterY))
    if (0, 0) == (spLeftX, spCenterY):
        logger.error("Cannot process %s" % filename)
        return

    pxPerMm = 393
    frameOriginX = int(spLeftX + (1.53 * pxPerMm))
    frameOriginY = int(spCenterY - (1.69 * pxPerMm))

    # FUDGE FACTOR for misaligned images
    #frameOriginY -= (122  + (.455 * pxPerMm))
#    frameOriginY -= ((.465 * pxPerMm))

    frameWidth = int(4.57 * pxPerMm)
    frameHeight = int(3.39 * pxPerMm)

    if frameWidth % 2 == 1:
        frameWidth += 1
    # crop and save
#    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
#        logger.error("Crop tile out of bounds %u x %u > %u x %u" % (frameOriginX + frameWidth, fcWidth, frameOriginY + frameHeight, fcHeight))
#        return

    if options.debug and bw_dir is not None:
        bwd = ImageDraw.Draw(imp)
        bwd.line((spLeftX, spCenterY, fcWidth, spCenterY), fill=0)
        bwd.line((spLeftX, spCenterY - (1.64 * pxPerMm),
            spLeftX, spCenterY + (1.64 * pxPerMm)), fill = 0)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        imp.save('%s/%s' % (bw_dir, os.path.basename(filename)))

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
    global options
    global arrToFind
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

    if options.mode not in ('8mm', 'super8'):
        logger.error("Invalid sprocket option %s" % options.sprocket)
        sys.exit(1)

    imgToFind = PILImage.open('%s/tofind.jpg' % os.path.dirname(os.path.abspath(__file__))).convert('L')
    arrToFind = scipy.misc.fromimage(imgToFind, flatten = True).astype(numpy.uint8)
    #arrFindIn[arrFindIn < 100] = 0
    #arrFindIn[arrFindIn >= 100] = 255

    processor = {'super8': processSuper8, '8mm': process8mm }[options.mode]
#    pdb.set_trace()

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

