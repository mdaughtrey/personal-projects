#!/usr/bin/python

from optparse import OptionsParser
from PIL import Image, ImageDraw
import sys
outpath = '/media/sf_vmshared/fb/'

parser = OptionParser('autocrop.py [-v] -i inputdir -o outputdir')

parser.add_option('-v', '--verbose', dest='verbose')
parser.add_option('-i', '--input-dir', dest='inputdir')
parser.add_option('-o', '--output-dir', dest='outputdir')

(options, args) = parser.parse_args()

if options.inputdir is None or options.outputdir is None:
		parser.print_help()
		sys.exit(1)

if options.inputdir == options.outdir:
		print 'Input and output directories must be different'
		parser.print_help()
		sys.exit(1)

def process8mm(filename):
		im = Image.open('%s/%s' % (options.inputdir, options.filename))
		if options.verbose:
				print '%s' % options.filename
		(imX, imY) = im.size
		xfile = im.copy().convert('L')
		xfile = xfile.point(lambda ii: ii > 190 and 255)
		lineX = imX / 5 * 4
		lineY = imY / 2 

		# find the bounding box of the upper sprocket hole
		upperCrop = xfile.copy()
		upperCrop = upperCrop.crop((lineX - 20, 0, imX, lineY))
		upperBox = upperCrop.getbbox()

		# find the bounding box of the lower sprocket hole
		lowerCrop = xfile.copy()
		lowerCrop = lowerCrop.crop((lineX - 20, lineY, imX, imY))
		lowerBox = lowerCrop.getbbox()

		# calculate the size and offset of the cropped image
		pxPerMm = (upperBox[3] - upperBox[1]) / 1.23
		centerFrameV = (lowerBox[1] - upperBox[3] + lineY)
		frameOriginX = upperBox[2] - (1.8 + 4.5 + .35) * pxPerMm + lineX
		frameOriginY = centerFrameV - (1.65 * pxPerMm)

		# crop and save
		theFrame = im.crop((int(frameOriginX), int(frameOriginY),
				 int(frameOriginX + 4.5 * pxPerMm), int(frameOriginY + 3.3 * pxPerMm)))
		theFrame.save('%s/%s' % (options.outputdir, options.filename))

process8mm()
