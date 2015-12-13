#!/usr/bin/python

from optparse import OptionParser
from PIL import Image, ImageDraw
import sys
import os
from glob import glob, iglob

debugdir='/mnt/imageinput/v3test/test8/autocropped'

greyed_dir = {True:'greyed', False: None}[os.path.isdir('greyed')]
bw_dir = {True:'bw', False: None}[os.path.isdir('bw')]
corner_dir = {True:'corner', False: None}[os.path.isdir('corner')]

options = {}

def findExtents(image):
    (top, bottom) = (0, 0)
    (sfWidth, sfHeight) = image.size
    startAt = sfHeight / 2
    sfSequence = list(image.getdata())

    compTo = []
    for jj in range(0, sfWidth):
        compTo.append(0)

#    if options.verbose:
#        print "Looking for top"
    for top in xrange(startAt * sfWidth, 0, -sfWidth):
#        if options.verbose:
#            print sfSequence[top:top + sfWidth]
        if compTo != sfSequence[top:top + sfWidth]:
            break

#    if options.verbose:
#        print "Looking for bottom"
    for bottom in xrange(startAt * sfWidth, sfWidth * sfHeight, sfWidth):
#        if options.verbose:
#            print sfSequence[top:top + sfWidth]
        if compTo != sfSequence[bottom:bottom + sfWidth]:
            break;

    return (top / 10, bottom / 10)

def findSprocketRight(sprockets, filename):
    (fcWidth, fcHeight) = sprockets.size
    sprockets = sprockets.crop((fcWidth / 4 * 3, 0, fcWidth, fcHeight / 4 ))
    (spWidth, spHeight) = sprockets.size
    spData = list(sprockets.getdata())
    spData.reverse()

    minVal = spWidth 
    maxRow = 0
    for ii in xrange(0, spWidth * spHeight, spWidth):
        try:
            matched = spData[ii:ii + spWidth].index(255)
            if  minVal > matched:
                maxRow = ii / spWidth
                minVal = matched
        except ValueError:
            pass
    
    if corner_dir is not None:
        spd = ImageDraw.Draw(sprockets)
        spd.line((spWidth - minVal, maxRow, spWidth, maxRow), fill=192)
        sprockets.save('%s/%s' % (corner_dir, os.path.basename(filename)))

    if options.verbose:
        print "findSprocketRight returns %u from row %u" % (minVal, maxRow)
    return minVal

def process8mm(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
    (fcWidth, fcHeight) = fullColor.size

    bw = fullColor.copy().convert('L')
    if greyed_dir is not None: 
        bw.save('%s/%s' % (greyed_dir, os.path.basename(filename)))
    bw = bw.point(lambda ii: ii > 150 and 255)

    sprockets = bw.copy()
    spRight = findSprocketRight(sprockets, filename)

    sfX = fcWidth - spRight - 200
    sprockets = sprockets.crop((sfX, 0, sfX + 10, fcHeight))
    (sfWidth, sfHeight) = sprockets.size
#    sprockets.save('tmp/%s' % os.path.basename(filename))


    (boxTop, boxBottom) = findExtents(sprockets)
    pxPerMm = (boxBottom - boxTop) / 2.58
    frameOriginX = int(fcWidth - spRight -  ((1.8 + 4.5 + .6) * pxPerMm))
    frameOriginY = int(boxTop - (.5 * pxPerMm))
    frameWidth = int((4.5 + 0.2)  * pxPerMm)
    frameHeight = int(3.6 * pxPerMm)

    if bw_dir is not None: 
        bwd = ImageDraw.Draw(bw)
        bwd.rectangle((sfX, boxTop, sfX + 10, boxBottom), fill=192)
        bwd.rectangle((fcWidth - spRight, boxTop, fcWidth - spRight + 10, boxBottom), fill=192)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))


#    lineX = fcWidth / 20 * 19
#    lineY = fcHeight / 2 
#
#    # find the bounding box of the upper sprocket hole
#    upperCrop = bw.copy()
#    upperCrop = upperCrop.crop((lineX, 0, fcWidth, lineY))
#    #upperCrop.save('%s/upper.jpg' % debugdir)
#    upperBox = list(upperCrop.getbbox())
#
#    upperBox[0] += lineX
#    upperBox[2] += lineX
#
#    # find the bounding box of the lower sprocket hole
#    lowerCrop = bw.copy()
#    lowerCrop = lowerCrop.crop((lineX, lineY, fcWidth, fcHeight))
#    #lowerCrop.save('%s/lower.jpg' % debugdir)
#    lowerBox = list(lowerCrop.getbbox())
#
#    lowerBox[0] += lineX
#    lowerBox[1] += lineY
#    lowerBox[2] += lineX
#    lowerBox[3] += lineY
#
##    if options.verbose:
##        print 'upperBox', upperBox
##        print 'lowerBox', lowerBox
#
#    # calculate the size and offset of the cropped image
#    if os.path.isdir(bw_dir): 
#        bwd = ImageDraw.Draw(bw)
##        bwd.line((int(frameOriginX), int(frameOriginY),
##             int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)),
##            fill = 255)
#        bwd.line((lineX, 0, lineX, fcHeight), fill = 255)
#        bwd.rectangle((frameOriginX, frameOriginY,
#				frameOriginX + frameWidth,
#				frameOriginY + frameHeight),
#				 fill = 140)
##        bwd.line((lineX + 1, 0, lineX + 1, fcHeight), fill = 0)
#        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))
    # crop and save
    fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
         int(frameOriginX + frameWidth),
         int(frameOriginY + frameHeight)))
    fullColor.save('%s/%s' % (outputpath, os.path.basename(filename)))

def process8mm2(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
    (fcWidth, fcHeight) = fullColor.size

    bw = fullColor.copy().convert('L')
    if greyed_dir is not None: 
        bw.save('%s/%s' % (greyed_dir, os.path.basename(filename)))
    bw = bw.point(lambda ii: ii > 80 and 255)
    lineX = fcWidth / 20 * 19
    lineY = fcHeight / 2 

    # find the bounding box of the upper sprocket hole
    upperCrop = bw.copy()
    upperCrop = upperCrop.crop((lineX, 0, fcWidth, lineY))
    #upperCrop.save('%s/upper.jpg' % debugdir)
    upperBox = list(upperCrop.getbbox())

    upperBox[0] += lineX
    upperBox[2] += lineX

    # find the bounding box of the lower sprocket hole
    lowerCrop = bw.copy()
    lowerCrop = lowerCrop.crop((lineX, lineY, fcWidth, fcHeight))
    #lowerCrop.save('%s/lower.jpg' % debugdir)
    lowerBox = list(lowerCrop.getbbox())

    lowerBox[0] += lineX
    lowerBox[1] += lineY
    lowerBox[2] += lineX
    lowerBox[3] += lineY

#    if options.verbose:
#        print 'upperBox', upperBox
#        print 'lowerBox', lowerBox

    # calculate the size and offset of the cropped image
    pxPerMm = (upperBox[3] - upperBox[1]) / 1.23

#	if options.verbose:
#	    print 'pxPerMm %u' % pxPerMm
    centerFrameV = ((lowerBox[1] - upperBox[3]) / 2) + upperBox[3]
    frameOriginX = upperBox[2] - ((1.5 + 4.5 + .1) * pxPerMm)
    frameOriginY = centerFrameV - (1.65 * pxPerMm)
#    if options.verbose:
#        print 'pxPerMm %f centerFrameV %u' % (pxPerMm, centerFrameV)
#        print 'frameOriginX %u frameOriginY %u' % (frameOriginX, frameOriginY)

    frameWidth = (4.5 + 0.3)  * pxPerMm
    frameHeight = 3.3 * pxPerMm
    if bw_dir is not None: 
        bwd = ImageDraw.Draw(bw)
#        bwd.line((int(frameOriginX), int(frameOriginY),
#             int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)),
#            fill = 255)
        bwd.line((lineX, 0, lineX, fcHeight), fill = 255)
        bwd.rectangle((frameOriginX, frameOriginY,
				frameOriginX + frameWidth,
				frameOriginY + frameHeight),
				 fill = 140)
#        bwd.line((lineX + 1, 0, lineX + 1, fcHeight), fill = 0)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))
    # crop and save
    fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
         int(frameOriginX + frameWidth),
         int(frameOriginY + frameHeight)))
    fullColor.save('%s/%s' % (outputpath, os.path.basename(filename)))

def main():
    global options
    parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')
    parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False)
    parser.add_option('-i', '--input-dir', dest='inputdir')
    parser.add_option('-o', '--output-dir', dest='outputdir')
    parser.add_option('-f','--filename', dest='filename')
    (options, args) = parser.parse_args()

#    if options.inputdir is None or options.outputdir is None:
#        parser.print_help()
#        sys.exit(1)
#
#    if options.inputdir == options.outputdir:
#        print 'Input and output directories must be different'
#        parser.print_help()
#        sys.exit(1)

    if options.filename is not None:
        process8mm(options.filename, options.outputdir)
    else:
        for ff in iglob('%s/*.JPG' % options.inputdir):
            process8mm(ff, options.outputdir)

main()

