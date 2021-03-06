#!/usr/bin/env python3

import os.path
import pdb
import sys
import scipy
from PIL import Image as PILImage # , ImageDraw, ImageFilter, ImageFile
from PIL import ImageOps

def main():
    if len(sys.argv) == 1:
        print("need a picture")
        return 1

    filename = sys.argv[1]
    base = os.path.basename(filename).split('.')[0]
    img = PILImage.open(filename).convert('L')
    img = ImageOps.invert(img)
    img = img.convert('1')
#    img2 = img.transpose(PILImage.ROTATE_90)
    img.save("{}_1bit.bmp".format(base))
    #img.transpose(PILImage.ROTATE_90).save("{}_1bit.bmp".format(base))
    dd = ','.join([hex(~x & 0xff) for x in img.tobytes()])
    open('{}.h'.format(base),'w').write('const unsigned char {}_data[] = {{{}}};\n'.format(base, dd))
#    flattened = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
#    scipy.misc.imsave('%s/%s/sprocket_%s' % (options.outputdir, sprockets_dir, os.path.basename(filename)), sprocket)


main()
