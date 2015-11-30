#!/usr/bin/python

from optparse import OptionParser
from PIL import Image, ImageDraw
import sys
import os
from glob import glob, iglob

debugdir='/mnt/imageinput/v3test/test8/autocropped'


options = {}

def process8mm(filename, outputpath):
    im = Image.open(filename)
    if options.verbose:
        print '%s' % filename
    (imX, imY) = im.size
    xfile = im.copy().convert('L')
    xfile.save('/mnt/imageinput/v3test/grayed.JPG')
    xfile = xfile.point(lambda ii: ii > 80 and 255)
    xfile.save('/mnt/imageinput/v3test/bw.JPG')
    lineX = imX / 5 * 4
    lineY = imY / 2 

#    id = ImageDraw.Draw(xfile)
#    id.line((lineX, 0, lineX, imY), fill = 255)
#    del id
#    xfile.save('/mnt/imageinput/v3test/line.JPG')

    # find the bounding box of the upper sprocket hole
    upperCrop = xfile.copy()
    upperCrop = upperCrop.crop((lineX, 0, imX, lineY))
    upperCrop.save('%s/upper.jpg' % debugdir)
    upperBox = list(upperCrop.getbbox())
    upperBox[0] += lineX
    upperBox[2] += lineX

    # find the bounding box of the lower sprocket hole
    lowerCrop = xfile.copy()
    lowerCrop = lowerCrop.crop((lineX, lineY, imX, imY))
    lowerCrop.save('%s/lower.jpg' % debugdir)
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
    centerFrameV = ((lowerBox[1] - upperBox[3]) / 2) + upperBox[3]
    frameOriginX = upperBox[2] - ((1.8 + 4.5 + .1) * pxPerMm)
    frameOriginY = centerFrameV - (1.65 * pxPerMm)
#    if options.verbose:
#        print 'pxPerMm %f centerFrameV %u' % (pxPerMm, centerFrameV)
#        print 'frameOriginX %u frameOriginY %u' % (frameOriginX, frameOriginY)

    # crop and save
    theFrame = im.crop((int(frameOriginX), int(frameOriginY), int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)))
    theFrame.save('%s/%s' % (outputpath, os.path.basename(filename)))

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

