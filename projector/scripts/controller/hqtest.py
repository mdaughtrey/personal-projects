#!/usr/bin/env python3

import  argparse
from    collections import deque
import  concurrent.futures
import  cv2
from    functools import partial
from    glob import glob, iglob
from    itertools import groupby
#import matplotlib.pyplot as plt
import  numpy as np
import  os
import  pdb
from    PIL import Image, ImageDraw, ImageFilter
import  scipy
from    scipy import ndimage
import  statistics as st

parser = argparse.ArgumentParser()
parser.add_argument('--project', required = True, dest='project', help='project name')
parser.add_argument('--root', required = True, dest='root', help='rootdir')
parser.add_argument('--format', required = True, dest='format', help='Output Format', default='png', choices = ['png', 'bmp'])
parser.add_argument('--nowrite', dest='nowrite', help='dont write the image files', action="store_true")
parser.add_argument('--overwrite', dest='overwrite', help='overwrite the output file', action="store_true")
parser.add_argument('--res', dest='res',  choices=['draft', 'hd', 'full'], help="resolution")
parser.add_argument('--serialize', dest='serialize', action='store_true', default=False, help='serialized operation')
parser.add_argument('--onefile', dest='onefile', help='Process one file')
parser.add_argument('--showwork', dest='showwork', action='store_true', default=False, help='Show intermediary cropfiles')
parser.add_argument('--showcrop', dest='showcrop', action='store_true', default=False, help='Show crop boxes')
config = parser.parse_args()

#dodraw = False
dodraw = True
#serial = False
#dodraw = True
# left upper right lower
#crop = (540, 530, 3036, 2484)
#489x 168x
# 2574w 1761h
crop = (489, 168, 489+2574, 168+1761)
#crop = (540+70, 530+50, 3036-70, 2484-30-350)
# 8mm 4.5 x 3.3mm
# S8 5.36 x 4.01
PxPerMm8mm = (crop[2] - crop[0])/4.5
PxPerMmS8 = {'draft':75, '1k':int(467/4.01), 'hd': int(1200/5.36), 'full':391}[config.res]

def roundIt(val): return int(val + val%2)
SprocketS8 = type('obj', (object,), {'h': roundIt(.91 * PxPerMmS8),
    'w': roundIt(1.14 * PxPerMmS8)})
FrameS8 = type('obj', (object,), {'w': roundIt(5.46 * PxPerMmS8),
    'h': roundIt(4.01 * PxPerMmS8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'w': roundIt(4.5 * PxPerMm8mm),
    'h': roundIt(3.4 * PxPerMm8mm)})


def findExtents0(matrix):
    (nrows, ncols) = matrix.shape
    test = np.full_like(matrix[0], 255)
    rstart = 0
    row = 0
    maxlen = 0
    for k, g in groupby(matrix, lambda x: np.array_equal(x, test)):
        leng = len(list(g))
        if k == True and leng > maxlen:
            maxlen = leng
            rstart = row
        row += leng
        
    return (0, rstart, ncols-1, rstart + maxlen)


def findExtents(matrix):
	matrix = np.asarray(matrix, dtype=np.uint8)
	hists = np.zeros(shape=(1, matrix.shape[1]), dtype=np.uint16)

	# Build cumulative histograms
	for rr in range(matrix.shape[0]):
		thisrow = np.copy(matrix[rr]) + hists[len(hists)-1] * matrix[rr]
		hists = np.vstack((hists, thisrow))

	# Find the max rectangle in each histogram
	# (x0, y0, x1, y1)
	rect = (0, 0, 0, 0)
	maxArea = 0
	for hh in range(len(hists)):
		col = 0
		for k,g in groupby(hists[hh], lambda x: x > 0):
			line = list(g)
			xlen = len(line)
			if k:
				xmin = np.amin(line)
				area = xmin * xlen
				if area > maxArea:
					maxArea = area
					rect = (col, hh-xmin, col+xlen, hh)
			col += xlen

	# (x0, y0, x1, y1)
	return rect

def tupleAdd(t0, t1):
    return tuple(map(sum, (zip(t0, t1))))

def savedebug(image, filename, tag):
    if filename:
        Image.fromarray(image).save(filename.replace('.png', '_{}.png'.format(tag)))

# Shape = Y,X
def findSprockets8mm(image):
#    pdb.set_trace()
    grey = image.convert('L')
    savedebug(grey, 'grey')
    flattened = np.asarray(grey, dtype=np.uint8)

    #sliceL = 550
    #sliceR = 600
    sliceL = 250
    sliceR = 300
    slice = ndimage.grey_erosion(flattened[:,sliceL:sliceR], size=(25,25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

    threshold = darkest + (lightest - darkest)/2
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 255

    if dodraw: # sprocketslice
        # paste slice
        x = Image.fromarray(slice)
        rect =  (sliceL, 0, sliceR, x.size[1])
        image.paste(x, rect)
        # left upper right lower
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#0ff000', width=1)
        draw.rectangle(tupleAdd(rect, (1,1,-1,-1)), outline='#ffffff', width=1)

    sprocketBoundary = int(flattened.shape[0]/4*3)
    upperSprocket = tupleAdd((sliceL, 0, sliceL, 0), findExtents(slice[:sprocketBoundary+50]))
    #lowerSprocket = tupleAdd((sliceL, sprocketBoundary-50, sliceL, sprocketBoundary-50),
    #    findExtents(slice[sprocketBoundary-50:]))
    #centerline = upperSprocket[3] + int((lowerSprocket[1] - upperSprocket[3])/2)
    #croprect = (740, centerline - (Frame8mm.h/2) - 60, 740 + Frame8mm.w,
    #        centerline + (Frame8mm.h/2))
    croprect = (0, centerline - (Frame8mm.h/2) - 60, grey.size[0],
            centerline + (Frame8mm.h/2))
    croprect = tupleAdd(croprect, (0,-200,-500,200))

    if dodraw:
        draw = ImageDraw.Draw(image)
        # upper sprocket
        draw.rectangle(upperSprocket, outline='#ffff00', width=1)
        draw.rectangle(tupleAdd(upperSprocket, (1,1,-1,-1)), outline='#000000', width=1)
        # lower sprocket
        draw.rectangle(lowerSprocket, outline='#ffff00', width=1)
        draw.rectangle(tupleAdd(lowerSprocket, (1,1,-1,-1)), outline='#000000', width=1)

        # Frame
        line = (0, centerline, image.size[0]-1, centerline)
        draw.line(line, fill=255, width=2)

        draw.rectangle(croprect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(croprect, (1,1,-1,-1)), outline='#ffffff', width=1)
    else:
        image = image.crop(croprect)

    return image

def findSprocketsS80(image): # backup
#    pdb.set_trace()
    grey = image.convert('L')
    savedebug(grey, 'grey')
    flattened = np.asarray(grey, dtype=np.uint8)

    sliceL = 250
    sliceR = 300
    slice = ndimage.grey_erosion(flattened[:,sliceL:sliceR], size=(25,25))

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

    threshold = darkest + (lightest - darkest)/2
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 255

    if dodraw: # sprocketslice
        # paste slice
        x = Image.fromarray(slice)
        rect =  (sliceL, 0, sliceR, x.size[1])
        image.paste(x, rect)
        # left upper right lower
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#0ff000', width=1)
        draw.rectangle(tupleAdd(rect, (1,1,-1,-1)), outline='#ffffff', width=1)

    sprocketBoundary = int(flattened.shape[0]/4*3)
    sprockCoords = tupleAdd((sliceL, 0, sliceL, 0), findExtents(slice[:sprocketBoundary+50]))
    centerline = int((sprockCoords[3] - sprockCoords[1])/2 + sprockCoords[1])
    open('{}/{}/centerline.dat'.format(config.root,config.project), 'a').write("{}\n".format(centerline))
    #lowerSprocket = tupleAdd((sliceL, sprocketBoundary-50, sliceL, sprocketBoundary-50),
    #    findExtents(slice[sprocketBoundary-50:]))
    #centerline = sprockCoords[3] + int((lowerSprocket[1] - sprockCoords[3])/2)
    #croprect = (740, centerline - (Frame8mm.h/2) - 60, 740 + Frame8mm.w,
    #        centerline + (Frame8mm.h/2))
    croprect = (0, centerline - (FrameS8.h/2) - 60, grey.size[0],
            centerline + (FrameS8.h/2))
#    croprect = tupleAdd(croprect, (0,-200,-500,200))

    if dodraw:
        draw = ImageDraw.Draw(image)
        # upper sprocket
        draw.rectangle(sprockCoords, outline='#ffff00', width=1)
        draw.rectangle(tupleAdd(sprockCoords, (1,1,-1,-1)), outline='#000000', width=1)
        # lower sprocket
#        draw.rectangle(lowerSprocket, outline='#ffff00', width=1)
#        draw.rectangle(tupleAdd(lowerSprocket, (1,1,-1,-1)), outline='#000000', width=1)

        # Frame
        line = (0, centerline, image.size[0]-1, centerline)
        draw.line(line, fill=255, width=2)

        draw.rectangle(croprect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(croprect, (1,1,-1,-1)), outline='#ffffff', width=1)
    else:
        pass
#        image = image.crop(croprect)

    return (image, centerline)

def findSprocketsS8(image, filename=None):
#    pdb.set_trace()
    grey = image.convert('L')
    #image = image.convert('L')
    flattened = np.asarray(grey, dtype=np.uint8)
    savedebug(flattened, filename, 'flattened')

    eroded = ndimage.grey_erosion(flattened, size=(5,5))
    savedebug(eroded, filename, 'eroded')

    sliceL = 0
    sliceR = int(PxPerMmS8 * (0.51 + 0.91))
    sliceW = sliceR - sliceL
    # Slice runs down the left, vertically clipped 1/2 vertical frame size top and bottom
    sliceY = int(FrameS8.h/2)
    slice = eroded[:,sliceL:sliceR]

    savedebug(slice, filename, 'slice')

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

#    threshold = darkest + (lightest - darkest)/2
    threshold=205
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 1

    if config.showwork:
        slice2 = np.copy(slice)
        slice2[slice2 == 1] = 255
        savedebug(slice2, filename, 'darklight')

    if filename:
        debugimage = image

    centerlines = []
    for ss in range(sliceL, sliceR, int(sliceW/5)):
        vstrip = slice[:,ss:ss+int(sliceW/10)]

        # rect x0,y0,x1,y1
        rect = findExtents(vstrip)
        if SprocketS8.h < (rect[3] - rect[1]):
            rect = tupleAdd((ss, 0, ss, 0), rect)
            centerlines.append(int(rect[1] + (rect[3]-rect[1])/2))

    if config.showwork:
        savedebug(np.asarray(debugimage), filename, 'strips')
    centerlines = [x for x in centerlines if x]
    if len(centerlines):
        yCenter = int(st.median(centerlines))
    else:
        yCenter = int(image.height/2)
        #yCenter = image.shape[0]/2


    # Now we have a Y-axis sprocket hole yCenter, find the X-axis sprocket hole yCenter
    hstrip = slice[yCenter - 50 : yCenter + 50]
    rect = findExtents(hstrip.transpose())
    xCenter = rect[1] + (rect[3] - rect[1])/2

    if config.showwork:
        open('{}/{}/centerline.dat'.format(config.root,config.project), 'a').write("{}\n".format(yCenter + sliceY))
    sprocketRect = (int(xCenter - SprocketS8.w/2), int(yCenter - SprocketS8.h/2),
        int(xCenter + SprocketS8.w/2), int(yCenter + SprocketS8.h/2))
    cropRect = (int(xCenter + SprocketS8.w/2), int(yCenter - FrameS8.h/2),
        int(xCenter + SprocketS8.w/2 + FrameS8.w), int(yCenter + FrameS8.h/2))

    if config.showcrop:
        draw = ImageDraw.Draw(image)
        # sprocket
        draw.rectangle(sprocketRect, outline='#ffff00', width=1)
        draw.rectangle(tupleAdd(sprocketRect, (1,1,-1,-1)), outline='#000000', width=1)

        # Frame
        line = (0, yCenter, image.size[0]-1, yCenter)
        draw.line(line, fill=255, width=2)

        draw.rectangle(cropRect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(cropRect, (1,1,-1,-1)), outline='#ffffff', width=1)
#        savedebug(np.asarray(image), filename, 'cropboxes')
    else:
        image = image.crop(cropRect)

    return image

def main():
    dirnum = 0
    while True:
        fromdir = f'{config.root}/{config.project}/{dirnum:03d}/'
        todir = '{root}/{proj}/{format}_{croptype}'.format(root = config.root,
            proj = config.project, 
            format = config.format,
            croptype = {True: "_showcrop", False: "cropped"}[config.showcrop])
        print(f'Dir {fromdir}', end='\n')
        if not os.path.exists(fromdir):
            return 0
        if not os.path.exists(todir):
            os.mkdir(todir)

        dirnum += 1
        for file in glob(f'{fromdir}/*.rgb'):
            print(file)
            outfile = '{dir}/{filename}'.format(dir=todir, filename = os.path.split(file)[1].replace('.rgb', '_cropped.') + config.format)
            if os.path.exists(outfile):
                continue
            image = Image.frombytes('RGB', (4064, 3040),open(file,'rb').read())
#            if dodraw:
#                draw = ImageDraw.Draw(image)
#                draw.rectangle(crop, outline='#000000', width=1)
#                draw.rectangle(tupleAdd(crop, (-1,-1,1,1)), outline='#ffffff', width=1)

#            image = findSprockets(image)
            image.save(outfile)

def threadrun(fromdir, todir, file):
    pdb.set_trace()
    outfilename='{:06d}a.{}'.format(int(os.path.basename(file)[:-5])+200, config.format)
    outfile = '{dir}/{filename}'.format(dir=todir, filename = outfilename)
    if os.path.exists(outfile) and (False == config.nowrite) and (False == config.overwrite):
        return

    res = {'draft':(640, 480), '1k':(1024,768), 'hd': (1920,1080), 'full':(4064,3040)}
    image = Image.frombytes('RGB', res[config.res],open(file,'rb').read())
    print(f'Processing {file}')
#    for b in range(16):
#        blurred = image.filter(ImageFilter.BoxBlur(b))
#        bfile = outfile.replace('.png', f'_{b}.png')
#        blurred.save(bfile)
#    image = cv2.GaussianBlur(image, {'width':4,'height':4}, 0, 0, cv2.BORDER_REFLECT_101)
    #image = findSprocketsS8(image, outfile)
    if config.showwork:
        image = findSprocketsS8(image, outfile)
    else:
        image = findSprocketsS8(image) # , outfile)
    if False == config.nowrite:
        image.save(outfile)


def mainthreaded():
    dirnum = 0
    while True:
        fromdir = f'{config.root}/{config.project}/{dirnum:03d}/'
        todir = '{root}/{proj}/{format}_{croptype}_{res}'.format(root = config.root,
            proj = config.project, 
            format = config.format,
            croptype = {True: "showcrop", False: "cropped"}[config.showcrop],
            res=config.res)
        #todir = '{}/{}/cropped_{}'.format(config.root, config.project, config.res)
        print(f'Dir {fromdir}', end='\n')
        if not os.path.exists(fromdir):
            return 0
        if not os.path.exists(todir):
            os.mkdir(todir)
        dirnum += 1
        bound = partial(threadrun, fromdir, todir)
        files =  [f'{fromdir}/{config.onefile}'] if config.onefile else glob(f'{fromdir}/*.rgb')
#        pdb.set_trace()
        if config.serialize:
            for file in files:
                bound(file)
        else:
            with concurrent.futures.ThreadPoolExecutor(max_workers = 4) as executor:
                executor.map(bound, files)
#
#
#        for file in glob(f'{fromdir}/*.rgb'):
#            print(file)
#            outfile = '{dir}/{filename}'.format(dir=todir, filename = os.path.split(file)[1].replace('.rgb', '_cropped.') + config.format)
#            if os.path.exists(outfile):
#                continue
#            image = Image.frombytes('RGB', (4064, 3040),open(file,'rb').read())
#            if dodraw:
#                draw = ImageDraw.Draw(image)
#                draw.rectangle(crop, outline='#000000', width=1)
#                draw.rectangle(tupleAdd(crop, (-1,-1,1,1)), outline='#ffffff', width=1)
#
#            image = findSprockets(image)
#            image.save(outfile)


mainthreaded()
#    pdb.set_trace()
#    ycrcb = cv2.cvtColor(np.array(image), cv2.COLOR_RGB2YCrCb)
#    np.save("out", ycrcb)
#    image.save(file.replace('.rgb', ''))
#    image.convert('YCbCr').save(file.replace('.rgb', '_cropped.yuv'), format='YCbCr')
