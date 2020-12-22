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
    sprocket = ndimage.grey_erosion(flattened[:,:300], size=(25,25))
#    if dodraw:
#        # left upper right lower
#        crop = (0, 0, 300, image.size[0]-1)
#        draw = ImageDraw.Draw(image)
#        draw.rectangle(crop, outline='#ff0000', width=1)
#        draw.rectangle(tuple(map(sum,(zip(crop, (1,1,-1,-1))))), outline='#ffffff', width=1)

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(sprocket)
    lightest = ndimage.maximum(sprocket)
#    pdb.set_trace()

    threshold = darkest + (lightest - darkest)/2
    sprocket[sprocket < threshold] = 0
    sprocket[sprocket >= threshold] = 255
    sprocketSlice = sprocket[:,50:200]

    if dodraw:
        # left upper right lower
        crop = (540, 0, 640, image.size[0]-1)
        draw = ImageDraw.Draw(image)
        draw.rectangle(crop, outline='#0ff000', width=1)
        draw.rectangle(tuple(map(sum,(zip(crop, (1,1,-1,-1))))), outline='#ffffff', width=1)

#    upperSprocket = findExtents("upper_%s" % os.path.basename(filename), sprocketSlice[:550])
#    lowerSprocket = findExtents("lower_%s" % os.path.basename(filename), sprocketSlice[1200:])

    return image

for file in glob('/media/sf_vproj/scans/hqtest/*.rgb'):
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
