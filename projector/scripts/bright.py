#!/usr/bin/python

from __future__ import division
import os
import sys
from PIL import Image as PILImage, ImageDraw, ImageFilter
import scipy
from scipy import ndimage
import numpy
from glob import glob, iglob
import sqlite3
import pdb

DBNAME='whitecount.db'

def initDb(dbName):
    conn = sqlite3.connect(dbName)
    cur = conn.cursor()
    cur.execute('''CREATE TABLE picdata (
     rawfile text,
     rawdir test,
     ignore boolean,
     size integer,
     whitecount integer,
     windex float)''')

    conn.commit()
    conn.close()

def saveToDb(dirname, filename, size, whitecount, index):
    if False == os.path.isfile(DBNAME):
        initDb(DBNAME)
    conn = sqlite3.connect(DBNAME)
    cur = conn.cursor()
    insert = "'%s','%s',%u,'FALSE',%u,%f" % (dirname,filename, size, whitecount, index)
    cur.execute('INSERT INTO picdata values (' + insert + ')')
    conn.commit()
    conn.close()

def getWhiteContent(filename):
    imp = PILImage.open(filename).convert('L')
    im = scipy.misc.fromimage(imp, flatten = True).astype(numpy.uint8)
    (height, width) = im.shape
#    im1 = ndimage.grey_erosion(im, size=(25, 25))

    im[im < 100] = 0
    im[im >= 100] = 255

    whiteCount = numpy.count_nonzero(im)
    index = whiteCount / (height * width)

    dirname = os.path.dirname(filename)
    saveToDb(dirname, filename, height * width, whiteCount, index)
    print "%s: Total %u white %u index %f" % (filename, height * width, whiteCount, index)

files = sorted(glob(sys.argv[1]))
for ff in files:
    getWhiteContent(ff)


