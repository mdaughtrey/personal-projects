#!/usr/bin/python

from optparse import OptionParser
from PIL import Image, ImageDraw, ImageFilter
import sys
import os
from glob import glob, iglob

debugdir='/mnt/imageinput/v3test/test8/autocropped'

greyed_dir = {True:'greyed', False: None}[os.path.isdir('greyed')]
bw_dir = {True:'bw', False: None}[os.path.isdir('bw')]
corner_dir = {True:'corner', False: None}[os.path.isdir('corner')]
sprockets_dir = {True:'sprockets', False: None}[os.path.isdir('sprockets')]

options = {}

FrameHeightMm = 3.3
FrameWidthMm = 4.5
PxPerMm8mm = 578
PxPerMmSuper8 = 370

def findSuper8Extents(image):
    (top, bottom) = (0, 0)
    (sfWidth, sfHeight) = image.size
    midway = sfWidth * sfHeight / 2 
    sfSequence = list(image.getdata())

    compTo = []
    for jj in range(0, sfWidth):
        compTo.append(0)

    for top in xrange(midway, 0 , -sfWidth):
        if sfSequence[top:top + sfWidth].count(0) > sfWidth / 2:
            break
#        if compTo == sfSequence[top:top + sfWidth]:

    for bottom in xrange(midway, sfHeight * sfWidth, sfWidth):
        if sfSequence[bottom:bottom + sfWidth].count(0) > sfWidth / 2:
            break;
#        if compTo == sfSequence[bottom:bottom + sfWidth]:

    return (top / sfWidth, bottom / sfWidth)

def find8mmExtents(image):
    (top, bottom) = (0, 0)
    (sfWidth, sfHeight) = image.size
    startAt = sfHeight / 2
    sfSequence = list(image.getdata())

    compTo = []
    for jj in range(0, sfWidth):
        compTo.append(0)

    for top in xrange(startAt * sfWidth, 0, -sfWidth):
        if compTo != sfSequence[top:top + sfWidth]:
            break

    for bottom in xrange(startAt * sfWidth, sfWidth * sfHeight, sfWidth):
        if compTo != sfSequence[bottom:bottom + sfWidth]:
            break;

    return (top / 10, bottom / 10)

def findSprocketRight(sprockets, filename):
    offset = 100
    (fcWidth, fcHeight) = sprockets.size
    sprockets = sprockets.crop(((fcWidth / 4 * 3) - offset, 0, fcWidth - offset, fcHeight / 4 ))
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
    return minVal + offset

def find8mmSprocketLeft(sprockets, filename):
    offset = 100
    (fcWidth, fcHeight) = sprockets.size
    sprockets = sprockets.crop((offset, 0, offset + (fcWidth / 4), fcHeight / 4))
    (spWidth, spHeight) = sprockets.size
    spData = list(sprockets.getdata())

    minVal = spWidth 
    maxRow = 0
    for ii in xrange(0, spWidth * spHeight, spWidth):
        try:
            matched = spData[ii:ii + spWidth].index(255)
            if minVal > matched:
                maxRow = ii / spWidth
                minVal = matched
        except ValueError:
            pass
    
    if corner_dir is not None:
        spd = ImageDraw.Draw(sprockets)
#        spd.line((spWidth - minVal, maxRow, spWidth, maxRow), fill=192)
        spd.line((0, maxRow, minVal, maxRow), fill=192)
        sprockets.save('%s/%s' % (corner_dir, os.path.basename(filename)))

    if options.verbose:
        print "find8mmSprocketLeft returns %u from row %u" % (minVal, maxRow)
    return minVal + offset

def findSuper8SprocketLeft(sprockets, filename):
    offset = 100
    (fcWidth, fcHeight) = sprockets.size
    sprockets = sprockets.crop((offset, int(fcHeight / 3),
         offset + (fcWidth / 4), int(fcHeight / 3 * 2)))
    (spWidth, spHeight) = sprockets.size
    spData = list(sprockets.getdata())

    minVal = spWidth 
    maxRow = 0
    for ii in xrange(0, spWidth * spHeight, spWidth):
        try:
            matched = spData[ii:ii + spWidth].index(255)
            if minVal > matched:
                maxRow = ii / spWidth
                minVal = matched
        except ValueError:
            pass
    
    if corner_dir is not None:
        spd = ImageDraw.Draw(sprockets)
#        spd.line((spWidth - minVal, maxRow, spWidth, maxRow), fill=192)
        spd.line((0, maxRow, minVal, maxRow), fill=192)
        sprockets.save('%s/%s' % (corner_dir, os.path.basename(filename)))

    if options.verbose:
        print "find8mmSprocketLeft returns %u from row %u" % (minVal, maxRow)
    return minVal + offset

def process8mmLeft(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
#    fullColor = fullColor.transpose(Image.FLIP_TOP_BOTTOM)
    (fcWidth, fcHeight) = fullColor.size

    bw = fullColor.copy().convert('L')
    bw = bw.filter(ImageFilter.MedianFilter)
    if greyed_dir is not None: 
        bw.save('%s/%s' % (greyed_dir, os.path.basename(filename)))
    bw = bw.point(lambda ii: ii > 110 and 255)

    sprockets = bw.copy()
    spLeft = find8mmSprocketLeft(sprockets, filename)

    sfX = spLeft + 200
    sprockets = sprockets.crop((sfX, 0, sfX + 10, fcHeight))
    (sfWidth, sfHeight) = sprockets.size

    (boxTop, boxBottom) = find8mmExtents(sprockets)
    pxPerMm = (boxBottom - boxTop) / 2.58
    #pxPerMm = PxPerMm8mm
    if pxPerMm < 100: # something's wrong
        print "pxPerMmm %u < 100" % pxPerMm
        sys.exit(1)

    frameOriginX = int(spLeft + ((1.8 + .3) * pxPerMm))

#    boxMid = ((boxBottom - boxTop) / 2) + boxTop
    boxMid = int((boxBottom + boxTop) / 2)
    frameOriginY = int(boxMid - ((FrameHeightMm /2 ) * pxPerMm))
#    frameOriginY = int(boxTop - (.4 * pxPerMm))

    frameWidth = int(4.5 * pxPerMm)
    frameHeight = int(3.3 * pxPerMm)

    if frameWidth % 2 == 1:
        frameWidth += 1

    #frameOriginX = int(fcWidth - spRight -  ((1.8 + 4.5 + .6) * pxPerMm))
    #frameOriginY = int(boxTop - (.4 * pxPerMm))
    #frameWidth = int((4.5)  * pxPerMm)
    #frameHeight = int(3.4 * pxPerMm)

    if bw_dir is not None: 
        bwd = ImageDraw.Draw(bw)
        bwd.rectangle((sfX, boxTop, sfX + 10, boxBottom), fill=192)
        bwd.rectangle((spLeft, boxTop, spLeft + 10, boxBottom), fill=192)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))

    # crop and save
    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
        print "Crop tile out of bounds %u x %u > %u > %u" % (frameOriginX + frameWidth, fcWidth,
        frameOriginY + frameHeight, fcHeight)
            
        sys.exit(1)
    try:
        fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
             int(frameOriginX + frameWidth),
             int(frameOriginY + frameHeight)))
        fullColor.save('%s/%s' % (outputpath, os.path.basename(filename)))

    except:
        print "Did not save %s/%s" % (outputpath, os.path.basename(filename) )
    
def process8mmRight(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
#    fullColor = fullColor.transpose(Image.FLIP_TOP_BOTTOM)
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


    (boxTop, boxBottom) = find8mmExtents(sprockets)
    pxPerMm = (boxBottom - boxTop) / 2.58
    frameOriginX = int(fcWidth - spRight -  ((1.8 + 4.5 + .6) * pxPerMm))
    frameOriginY = int(boxTop - (.4 * pxPerMm))
    frameWidth = int((4.8)  * pxPerMm)
    frameHeight = int(3.4 * pxPerMm)
    if frameWidth % 2 == 1:
        frameWidth += 1

    if bw_dir is not None: 
        bwd = ImageDraw.Draw(bw)
        bwd.rectangle((sfX, boxTop, sfX + 10, boxBottom), fill=192)
        bwd.rectangle((fcWidth - spRight, boxTop, fcWidth - spRight + 10, boxBottom), fill=192)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))

    # crop and save
    fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
         int(frameOriginX + frameWidth),
         int(frameOriginY + frameHeight)))
    fullColor.save('%s/%s' % (outputpath, os.path.basename(filename)))

def processSuper8Left(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
#    fullColor = fullColor.transpose(Image.FLIP_TOP_BOTTOM)
    (fcWidth, fcHeight) = fullColor.size

    bw = fullColor.copy().convert('L')
    bw = bw.filter(ImageFilter.MedianFilter)
    if greyed_dir is not None: 
        bw.save('%s/%s' % (greyed_dir, os.path.basename(filename)))
    bw = bw.point(lambda ii: ii > 110 and 255)

    sprockets = bw.copy()
    spLeft = findSuper8SprocketLeft(sprockets, filename)

    sfX = spLeft + 100
    sprockets = sprockets.crop((sfX, 0, sfX + 10, fcHeight))
    if sprockets_dir is not None: 
        sprockets.save('%s/%s' % (sprockets_dir, os.path.basename(filename)))
    (sfWidth, sfHeight) = sprockets.size

    (boxTop, boxBottom) = findSuper8Extents(sprockets)
    pxPerMm = PxPerMmSuper8
    #pxPerMm = (boxBottom - boxTop) / 1.14 
    #pxPerMm = PxPerMm8mm
#    if pxPerMm < 100: # something's wrong
#        print "pxPerMmm %u < 100" % pxPerMm
#        sys.exit(1)

    frameOriginX = int(spLeft + ((.91 + .2 ) * pxPerMm))

#    boxMid = ((boxBottom - boxTop) / 2) + boxTop
    boxMid = int((boxBottom + boxTop) / 2)
    if options.verbose:
        print 'pxPerMm %u top %u mid %u bottom %u' % (pxPerMm, boxTop, boxMid, boxBottom)
    frameOriginY = int(boxMid - ((4.01 / 2) * pxPerMm))
# FUDGE FACTOR for misaligned images
    #frameOriginY = int(boxTop - (.4 * pxPerMm))
    frameOriginY -= 122

    frameWidth = int(5.46 * pxPerMm)
    frameHeight = int(4.01 * pxPerMm)

    if frameWidth % 2 == 1:
        frameWidth += 1

    #frameOriginX = int(fcWidth - spRight -  ((1.8 + 4.5 + .6) * pxPerMm))
    #frameOriginY = int(boxTop - (.4 * pxPerMm))
    #frameWidth = int((4.5)  * pxPerMm)
    #frameHeight = int(3.4 * pxPerMm)
    if options.verbose:
        print 'frame W %u H %u X %u Y %u' % (frameWidth, frameHeight, frameOriginX, frameOriginY)


    if bw_dir is not None: 
        bwd = ImageDraw.Draw(bw)
        bwd.rectangle((sfX, boxTop, sfX + 10, boxBottom), fill=192)
        bwd.rectangle((spLeft, boxTop, spLeft + 10, boxBottom), fill=192)
        bwd.rectangle((frameOriginX, frameOriginY,
            frameOriginX + frameWidth,
            frameOriginY + frameHeight), fill=192)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))

    # crop and save
    if ((frameOriginX + frameWidth) > fcWidth) or ((frameOriginY + frameHeight) > fcHeight):
        print "Crop tile out of bounds %u x %u > %u > %u" % (frameOriginX + frameWidth, fcWidth,
        frameOriginY + frameHeight, fcHeight)
            
        sys.exit(1)
    try:
        fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY),
             int(frameOriginX + frameWidth),
             int(frameOriginY + frameHeight)))
        fullColor.save('%s/%s' % (outputpath, os.path.basename(filename)))

    except:
        print "Did not save %s/%s" % (outputpath, os.path.basename(filename) )
    

def main():
    global options
    parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')
    parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False)
    parser.add_option('-i', '--input-dir', dest='inputdir')
    parser.add_option('-o', '--output-dir', dest='outputdir')
    parser.add_option('-f','--filename', dest='filename')
    parser.add_option('-s','--sprocket', dest='sprocket', default='left')
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

    if options.sprocket not in ('left', 'right'):
        print "Invalid sprocket option %s" % options.sprocket
        sys.exit(1)

    if options.mode not in ('8mm', 'super8'):
        print "Invalid sprocket option %s" % options.sprocket
        sys.exit(1)

    if options.filename is not None:
        if '8mm' == options.mode:
            if 'left' == options.sprocket:
                process8mmLeft(options.filename, options.outputdir)
            else:
                process8mmRight(options.filename, options.outputdir)
        else:
            processSuper8Left(options.filename, options.outputdir)

    else:
        for ff in iglob('%s/*.JPG' % options.inputdir):
            process8mm(ff, options.outputdir)

main()

