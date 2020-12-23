#!/usr/bin/env python3

from glob import glob, iglob
from itertools import groupby
from PIL import Image, ImageDraw
import pdb
import scipy
from scipy import ndimage
import numpy as np

dodraw = True
# left upper right lower
crop = (540, 530, 3036, 2484)
PxPerMm8mm = 391
PxPerMmSuper8 = 391

def roundIt(val): return int(val + val%2)
SprocketSuper8 = type('obj', (object,), {'h': roundIt(.91 * PxPerMmSuper8),
    'w': roundIt(1.14 * PxPerMmSuper8)})
FrameSuper8 = type('obj', (object,), {'w': roundIt(5.46 * PxPerMmSuper8),
    'h': roundIt(4.01 * PxPerMmSuper8)})

Sprocket8mm = type('obj', (object,), {'h': roundIt(1.83 * PxPerMm8mm),
    'w': roundIt(1.27 * PxPerMm8mm)})
Frame8mm = type('obj', (object,), {'w': roundIt(4.37 * PxPerMm8mm),
    'h': roundIt(3.28 * PxPerMm8mm)})


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
                    area_max = (area, [(r-dh, c-minw+1, r, c)])
    return area_max

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

def findSprockets(image):
    grey = image.convert('L')
    flattened = np.asarray(grey)
#    flattened = scipy.misc.fromimage(imp, flatten = True).astype(np.uint8)
    sliceL = 550
    sliceR = 600
    slice = ndimage.grey_erosion(flattened[:,sliceL:sliceR], size=(25,25))
#    slice = sprocket[:,350:400]
    if dodraw:
        x = Image.fromarray(slice)
        image.paste(x.convert(image.mode), (sliceL, 0, sliceL + x.size[0], x.size[1]))
#        # left upper right lower
#        crop = (0, 0, 300, image.size[0]-1)
#        draw = ImageDraw.Draw(image)
#        draw.rectangle(crop, outline='#ff0000', width=1)
#        draw.rectangle(tuple(map(sum,(zip(crop, (1,1,-1,-1))))), outline='#ffffff', width=1)

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

    threshold = darkest + (lightest - darkest)/2
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 255
#    slice = sprocket[:,50:200]

    if dodraw: # sprocketslice
        # left upper right lower
        rect = (sliceL, 0, sliceR, slice.shape[0]-1)
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#0ff000', width=1)
        draw.rectangle(tuple(map(sum,(zip(rect, (1,1,-1,-1))))), outline='#ffffff', width=1)

    pdb.set_trace()
    upperSprocket = findExtents2(slice[:1300])
    lowerSprocket = findExtents2(slice[2300:])
    if dodraw:
#        rcrc
        # left upper right lower
        rect = [upperSprocket[1][0][x] for x in (1,0,3,2)]
        rect = tuple(map(sum,(zip(rect, (sliceL,0,sliceL,0)))))
        draw = ImageDraw.Draw(image)
        draw.rectangle(rect, outline='#ff0000', width=1)
        draw.rectangle(tuple(map(sum,(zip(rect, (1,1,-1,-1))))), outline='#ffffff', width=1)

    return image

#for file in glob('/media/sf_vproj/scans/hqtest/*.rgb'):
for file in glob('/home/mattd/Documents/hqtest/*.rgb'):
    print(file)
    image = Image.frombytes('RGB', (4064, 3040),open(file,'rb').read())
    if dodraw:
        draw = ImageDraw.Draw(image)
        draw.rectangle(crop, outline='#000000', width=1)
        draw.rectangle(tuple(map(sum,(zip(crop, (-1,-1,1,1))))), outline='#ffffff', width=1)
    else:
        image = image.crop(crop)

    image = findSprockets(image)
    #image.crop(crop).save(file.replace('.rgb', '_cropped.png'))
    image.save(file.replace('.rgb', '_cropped.png'))
#    sys.exit(0)
