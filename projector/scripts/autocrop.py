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

greyed_dir = {True:'greyed', False: None}[os.path.isdir('greyed')]
bw_dir = {True:'bw', False: None}[os.path.isdir('bw')]
corner_dir = {True:'corner', False: None}[os.path.isdir('corner')]
sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('sprockets')]
eroded_dir = {True:'eroded', False: None}[os.path.isdir('eroded')]

options = {}

FrameHeightMm = 3.3
FrameWidthMm = 4.5
PxPerMm8mm = 578
PxPerMmSuper8 = 370

def findSuper8Sprocket(image, filename):
    (imX, imY) = image.size
    fromLeft = {} 
    fromRight = {}
    leftExtent = {}
    imgData = list(image.getdata())
    # Scan each row from the left looking for white, store in fromLeft
    # Then continue to scan for black, store in leftExtent
    for ii in xrange(0, imX * imY - imX, imX):
        row = ii / imX
        try:
            fromLeft[row] = imgData[ii:ii + imX].index(255)
        except ValueError:
            pass

        try:
            fromRight[row] = imgData[ii + imX:ii:-1].index(0)
            #leftExtent[row] = imgData[ii + fromLeft[row] + 10:ii + imX].index(0)
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

        image.save('%s/%s' % (sprockets_dir, os.path.basename(filename)))

    # candidateRows contain white areas that could be a sprocket
    candidateRows = {kk for kk, vv in leftExtent.iteritems() if vv > 150}
    candidateRanges = []
    for kk, gg in groupby(enumerate(candidateRows), lambda (i,x): i - x):
        group = map(itemgetter(1), gg)
        candidateRanges.append((group[0], group[-1]))

    if options.debug:
        print "%u candidate ranges" % len(candidateRanges)

    # candidateRanges is a list of candidate row ranges
    # look for the one that's closest to the middle
    nominatedRange = ()
    closest = imY
    for range in candidateRanges:
        distance = abs((imY / 2) - (sum(range) / 2))
        if options.debug:
            print "(%u,%u) distance %u closest %u" % (range[0], range[1], distance, closest)
        if distance < closest:
            closest = distance
            nominatedRange = range

    if 0 == len(nominatedRange):
        if options.debug:
            print "No nominated range found!"
        return (0, 0)

    # return center of sprocket
    avgY = sum(nominatedRange) / 2
    return (fromLeft[avgY], avgY)
    #return (fromLeft[avgY] + (leftExtent[avgY]/2), avgY)

def processSuper8Left(filenames, outputpath):
    files = filenames.split(',')
    # Select the midrange image for figuring out the cropping
    filename = files[1]
    if 3 != len(files):
        print "Need three filenames"
        sys.exit(1)

    imp = PILImage.open(filename).convert('L')
    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    (fcWidth, fcHeight) = im.shape
    im = im[:,:400]
    im1 = ndimage.grey_erosion(im, size=(25, 25))

    im1[im1 < 100] = 0
    im1[im1 >= 100] = 255
    if options.debug and eroded_dir is not None:
        scipy.misc.imsave('eroded/%s' % os.path.basename(filename), im1)

    im1Image = scipy.misc.toimage(im1)
    (spLeftX, spCenterY) = findSuper8Sprocket(im1Image, filename)
    if options.debug:
        print "%s leftX %u centerY %u" % (filename, spLeftX, spCenterY)
    if (0, 0) == (spLeftX, spCenterY):
        print "Cannot process %s" % filename
        return
    pxPerMm = PxPerMmSuper8
    frameOriginX = int(spLeftX + ((.91 + .1) * pxPerMm))
    frameOriginY = int(spCenterY - (2.015 * pxPerMm))

    # FUDGE FACTOR for misaligned images
    #frameOriginY -= (122  + (.455 * pxPerMm))
    frameOriginY -= ((.455 * pxPerMm))

    frameWidth = int(5.46 * pxPerMm)
    frameHeight = int(4.01 * pxPerMm)

    if frameWidth % 2 == 1:
        frameWidth += 1
    # crop and save
    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
        print "Crop tile out of bounds %u x %u > %u x %u" % (frameOriginX + frameWidth, fcWidth, frameOriginY + frameHeight, fcHeight)
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
            print "Did not save %s/%s" % (outputpath, os.path.basename(iFile))

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

def main():
    global options
    parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')
    parser.add_option('-s', '--debug', dest='debug', help='Save intermediary images', action='store_true')
    parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False)
    parser.add_option('-i', '--input-dir', dest='inputdir')
    parser.add_option('-o', '--output-dir', dest='outputdir')
    parser.add_option('-f','--filenames', dest='filenames', help='Three filenames, comma separated')
    parser.add_option('-m','--mode', dest='mode', default='8mm')
    (options, args) = parser.parse_args()

#    if options.inputdir is None or options.outputdir is None:
#        parser.print_help()
#        sys.exit(1)
#
#    if options.inputdir == options.outputdir:
#        print 'Input and output directories must be different'
#        parser.print_help()
#        sys.exit(1)

    if options.mode not in ('8mm', 'super8'):
        print "Invalid sprocket option %s" % options.sprocket
        sys.exit(1)

    if options.filenames is not None:
        processSuper8Left(options.filenames, options.outputdir)
    else:
        files = glob('%s/*.JPG' % options.inputdir)
        for ii in xrange(0, len(files) - 3, 3):
            triple = ','.join(files[ii:ii+3])
            processSuper8Left(triple, options.outputdir)

#            process8mm(ff, options.outputdir)
#        if '8mm' == options.mode:
#            if 'left' == options.sprocket:
#                process8mmLeft(options.filenames, options.outputdir)
#            else:
#                process8mmRight(options.filenames, options.outputdir)
#        else:
#

main()

