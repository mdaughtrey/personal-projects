#!/usr/bin/python

from optparse import OptionParser
from PIL import Image, ImageDraw
import sys
import os
from glob import glob, iglob

debugdir='/mnt/imageinput/v3test/test8/autocropped'
greyed_dir='greyed'
bw_dir='bw'


options = {}

def process8mm(filename, outputpath):
    fullColor = Image.open(filename)
    if options.verbose:
        print '%s' % filename
    (fcWidth, fcHeight) = fullColor.size

    bw = fullColor.copy().convert('L')
    if os.path.isdir(greyed_dir): 
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
    centerFrameV = ((lowerBox[1] - upperBox[3]) / 2) + upperBox[3]
    frameOriginX = upperBox[2] - ((1.8 + 4.5 + .1) * pxPerMm)
    frameOriginY = centerFrameV - (1.65 * pxPerMm)
#    if options.verbose:
#        print 'pxPerMm %f centerFrameV %u' % (pxPerMm, centerFrameV)
#        print 'frameOriginX %u frameOriginY %u' % (frameOriginX, frameOriginY)

    if os.path.isdir(bw_dir): 
        bwd = ImageDraw.Draw(bw)
        bwd.line((int(frameOriginX), int(frameOriginY),
             int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)),
            fill = 255)
        bwd.line((lineX, 0, lineX, fcHeight), fill = 255)
        bwd.line((lineX + 1, 0, lineX + 1, fcHeight), fill = 0)
        bw.save('%s/%s' % (bw_dir, os.path.basename(filename)))
    # crop and save
    fullColor = fullColor.crop((int(frameOriginX), int(frameOriginY), int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)))
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

