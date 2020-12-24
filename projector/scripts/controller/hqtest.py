#!/usr/bin/env python3

from glob import glob, iglob
from itertools import groupby
from PIL import Image, ImageDraw
import pdb
import scipy
from scipy import ndimage
import numpy as np

dodraw = False
# left upper right lower
crop = (540, 530, 3036, 2484)
# 2974-776 = 2198/4.5
PxPerMm8mm = 488
PxPerMmSuper8 = 391


def roundIt(val): return int(val + val%2)
SprocketSuper8 = type('obj', (object,), {'h': roundIt(.91 * PxPerMmSuper8),
    'w': roundIt(1.14 * PxPerMmSuper8)})
FrameSuper8 = type('obj', (object,), {'w': roundIt(5.46 * PxPerMmSuper8),
    'h': roundIt(4.01 * PxPerMmSuper8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'w': roundIt(4.5 * PxPerMm8mm),
    'h': roundIt(3.8 * PxPerMm8mm)})


def findExtents2(data):
    (nrows, ncols) = data.shape
#    arrfile = open("/tmp/%s%s.bin" % (tag, os.getpid()),"wb")
#    arrfile.write(struct.pack("II", *data.shape))
#    data.tofile(arrfile)
#    arrfile.close()
    w = np.zeros(dtype=np.uint8, shape=data.shape)
    h = np.zeros(dtype=np.uint8, shape=data.shape)
    area_max = (0,[])
    for r in range(nrows):
#        print(f'Row {r}', end='\n')
        for c in range(ncols):
            if data[r][c] == 0:             # skip black pixels
                continue
            if r == 0:
                h[r][c] = 1                 # new row, set left extent
            else:
                h[r][c] = h[r-1][c]+1       # 
            if c == 0:
                w[r][c] = 1
            else:
                w[r][c] = w[r][c-1]+1
            minw = w[r][c]
            for dh in range(h[r][c]):
                minw = min(minw, w[r-dh][c])
                area = (dh+1)*minw
                if area > area_max[0]:
                    print(f'New area {area} at row {r}', end='\n')
                    if r == 797:
                        pdb.set_trace()
                    area_max = (area, [(r-dh, c-minw+1, r, c)])
    pdb.set_trace()
    return area_max


def findExtents3(matrix):
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

#def findExtents(tag, data):
#    (nrows, ncols) = data.shape
#    filename = "/tmp/%s%s.bin" % (tag, os.getpid())
#    arrfile = open(filename,"wb")
#    arrfile.write(struct.pack("II", *data.shape))
#    data.tofile(arrfile)
#    arrfile.close()
#    runargs = ['/media/sf_vproj/scans/software/a.out', '-debug', filename]
#    logger.debug("Calling %s" % ' '.join(runargs))
#    extents = subprocess.check_output(runargs).decode(sys.stdout.encoding).strip().split(' ')
#    os.remove(filename)
#    return dict(zip(['top','bottom','right'],[int(xx) for xx in extents]))

def tupleAdd(t0, t1):
    return tuple(map(sum, (zip(t0, t1))))

# Shape = Y,X
def findSprockets(image):
    grey = image.convert('L')
    flattened = np.asarray(grey, dtype=np.uint8)
#    flattened = scipy.misc.fromimage(imp, flatten = True).astype(np.uint8)
    sliceL = 550
    sliceR = 600
    slice = ndimage.grey_erosion(flattened[:,sliceL:sliceR], size=(25,25))
#    slice = sprocket[:,350:400]
#    if dodraw:
#        #image.paste(x.convert(image.mode), rect)
#        # left upper right lower
#        crop = (0, 0, 300, image.size[0]-1)
#        draw = ImageDraw.Draw(image)
#        draw.rectangle(tuple(map(sum,(zip(rect, (1,1,-1,-1))))), outline='#808080', width=1)

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

    threshold = darkest + (lightest - darkest)/2
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 255
#    slice = sprocket[:,50:200]

    if dodraw: # sprocketslice
        # paste slice
        x = Image.fromarray(slice)
        rect =  (sliceL, 0, sliceR, x.size[1])
        image.paste(x, rect)
        # left upper right lower
        #rect = (sliceL, 0, sliceR, slice.shape[0]-1)
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#0ff000', width=1)
        draw.rectangle(tuple(map(sum,(zip(rect, (1,1,-1,-1))))), outline='#ffffff', width=1)

#    pdb.set_trace()
    upperSprocket = tupleAdd((sliceL, 0, sliceL, 0), findExtents3(slice[:1300]))
    lowerSprocket = tupleAdd((sliceL, 2200, sliceL, 2200),
        findExtents3(slice[2200:]))
    centerline = upperSprocket[3] + int((lowerSprocket[1] - upperSprocket[3])/2)
    croprect = (sliceR+150, centerline - (Frame8mm.h/2), sliceR + Frame8mm.w + 200,
            centerline + (Frame8mm.h/2))

    if dodraw:
        draw = ImageDraw.Draw(image)
        # upper sprocket
        draw.rectangle(upperSprocket, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(upperSprocket, (1,1,-1,-1)), outline='#ffffff', width=1)
        # lower sprocket
        draw.rectangle(lowerSprocket, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(lowerSprocket, (1,1,-1,-1)), outline='#ffffff', width=1)

        # Frame
        line = (0, centerline, image.size[0]-1, centerline)
        draw.line(line, fill=255, width=2)

        draw.rectangle(croprect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(croprect, (1,1,-1,-1)), outline='#ffffff', width=1)
    else:
        image = image.crop(croprect)

    return image

#for file in glob('/media/sf_vproj/scans/hqtest/*.rgb'):
for file in glob('/home/mattd/Documents/hqtest/*.rgb'):
    print(file)
    image = Image.frombytes('RGB', (4064, 3040),open(file,'rb').read())
    if dodraw:
        draw = ImageDraw.Draw(image)
        draw.rectangle(crop, outline='#000000', width=1)
        draw.rectangle(tuple(map(sum,(zip(crop, (-1,-1,1,1))))), outline='#ffffff', width=1)
#    else:
#        image = image.crop(crop)

    image = findSprockets(image)
    #image.crop(crop).save(file.replace('.rgb', '_cropped.png'))
    image.save(file.replace('.rgb', '_cropped.png'))
#    sys.exit(0)
