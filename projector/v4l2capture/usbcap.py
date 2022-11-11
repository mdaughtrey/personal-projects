#!/usr/bin/env python3
#./opencvcap.py --serialport /dev/ttyACM0 --camindex 0 --film s8 --framesto ~/share/opencvcap0 --startdia 140 --enddia 80 --res draft

import  argparse
import  cv2
from    glob import glob, iglob
from    itertools import groupby
import  logging
from    logging.handlers import RotatingFileHandler
import  math
import  numpy as np
import  os
import  pdb
from    PIL import Image, ImageDraw, ImageFilter
from    scipy import ndimage
import  serial
from    Tension import Tension
import  time

res = [(640,480),(3264,2448),(2592,1944),(1920,1080),(1600,1200),(1280,720),(960,540),(848,480),(640,360),(424,240),(320,240),(320,180),(640,480)]


def proc_commandline():
    parser = argparse.ArgumentParser()
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8')
    parser.add_argument('--frames', dest='frames', type=int, default=-1, help='Number of frames to capture')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
#    parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
    return parser.parse_args()

def setlogging(config):
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('hqcap')
    fileHandler = logging.FileHandler(filename = './usbcap.log')
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)
    return logger

def serwaitfor(port, text1, text2, logger):
    accum = b''
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        try:
            accum += port.read()
        except:
            logger.debug("serwaitfor timeout")
        #logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return (0, text1)
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return (1, text2)

def init(config, logger):
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    numframes += 1000
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))
    map (lambda x: x * 3, tension)

    global lastTension
    lastTension = tension[0]

    logger.info("Opening %s" % config.serdev)
    serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
    if serdev.isOpen():
        serdev.close()
    serdev.open()
    serdev.write(b' ')
    wait = serwaitfor(serdev, b'{State:Ready}', b'No', logger)
#    if b'{State:Ready}' != portWaitFor(serPort, b'{State:Ready}'):
#        logger.debug("Init failed")
#        sys.exit(1)
#    interval = 20
    #message = f'{lastTension}tl1d2D{interval}e0E6000o'.encode()
    message = f'{lastTension}tl1d2D0E6000o'.encode()
    logger.info(message)
    serdev.write(message)
    #serdev.write(str.encode(f'{lastTension}tl1d2D{interval}e0E6000o'))
#    portWaitFor(serPort, b'{State:Ready}')
#    if False == config.noled:
#        serPort.write(b'l')
#    serPort.write(b'c%st' % {'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]) 
#    serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film], tension[0]).encode('utf-8'))
    #serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film].encode('utf-8'), tension[0]))
#    portWaitFor(serPort, b'{pt:')
    return serdev

def get_most_recent_frame(config):
    files = sorted(glob("{0}/????????.bmp".format(config.framesto)))
    if len(files):
        frames =  [int(os.path.basename(os.path.splitext(f)[0])) for f in files]
        frame = 1+ sorted(frames)[::-1][0]
    else:
        frame = 0
    return frame

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

def findSprocketsS8(logger, image, draw = False):
    grey = image.convert('L')
    #image = image.convert('L')
    flattened = np.asarray(grey, dtype=np.uint8)
    #savedebug(flattened, filename, 'flattened')

    eroded = ndimage.grey_erosion(flattened, size=(5,5))
    #savedebug(eroded, filename, 'eroded')

    sliceL = 0
    sliceR = int(image.size[0]/40)
#    sliceR = int(PxPerMmS8 * (0.51 + 0.91))
#    sliceW = sliceR - sliceL
    # Slice runs down the left, vertically clipped 1/2 vertical frame size top and bottom
    #sliceY = int(FrameS8.h/2)
    slice = eroded[:,sliceL:sliceR]

    #savedebug(slice, filename, 'slice')

    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

#    threshold = darkest + (lightest - darkest)/2
    threshold=205
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 1

#    if config.showwork:
#        slice2 = np.copy(slice)
#        slice2[slice2 == 1] = 255
#        savedebug(slice2, filename, 'darklight')

#    if filename:
#        debugimage = image

    rect = findExtents(slice)
#    centerlines = []
#    for ss in range(sliceL, sliceR, int(sliceW/5)):
#        vstrip = slice[:,ss:ss+int(sliceW/10)]
#
#        # rect x0,y0,x1,y1
#        rect = findExtents(vstrip)
#        if SprocketS8.h < (rect[3] - rect[1]):
#            rect = tupleAdd((ss, 0, ss, 0), rect)
#            centerlines.append(int(rect[1] + (rect[3]-rect[1])/2))

#    if config.showwork:
#        savedebug(np.asarray(debugimage), filename, 'strips')
#    centerlines = [x for x in centerlines if x]
#    if len(centerlines):
#        yCenter = int(st.median(centerlines))
#    else:
#        yCenter = image.shape[0]/2


    # Now we have a Y-axis sprocket hole yCenter, find the X-axis sprocket hole yCenter
#    hstrip = slice[yCenter - 50 : yCenter + 50]
	# (x0, y0, x1, y1)
    #rect = findExtents(hstrip.transpose())
#    pdb.set_trace()
    yCenter = rect[1] + (rect[3] - rect[1])/2
    delta = abs(yCenter-image.size[1]/2)
    threshold =  int(image.size[1]/20)
    logger.debug(f'yCenter {yCenter} delta {delta} threshold {threshold}')
    if abs(yCenter-image.size[1]/2) > int(image.size[1]/20):
        logger.debug('rejecting frame')
        return None

    xCenter = image.size[1]/2

    cropRect = (xclip := int(image.size[0]/10), yclip := image.size[0]/20, int(image.size[1]-xclip), int(image.size[1]-yclip))

#    if config.showwork:
#        open('{}/{}/centerline.dat'.format(config.root,config.project), 'a').write("{}\n".format(yCenter + sliceY))
#    sprocketRect = (int(xCenter - SprocketS8.w/2), int(yCenter - SprocketS8.h/2),
#        int(xCenter + SprocketS8.w/2), int(yCenter + SprocketS8.h/2))
#    cropRect = (int(xCenter + SprocketS8.w/2), int(yCenter - FrameS8.h/2),
#        int(xCenter + SprocketS8.w/2 + FrameS8.w), int(yCenter + FrameS8.h/2))

    if draw:
        def tupleAdd(t0, t1):
            return tuple(map(sum, (zip(t0, t1))))
        draw = ImageDraw.Draw(image)
#        # sprocket
#        draw.rectangle(sprocketRect, outline='#ffff00', width=1)
#        draw.rectangle(tupleAdd(sprocketRect, (1,1,-1,-1)), outline='#000000', width=1)
#
#        # Frame
#        line = (0, yCenter, image.size[0]-1, yCenter)
#        draw.line(line, fill=255, width=2)
#
        draw.rectangle(cropRect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(cropRect, (1,1,-1,-1)), outline='#ffffff', width=1)
##        savedebug(np.asarray(image), filename, 'cropboxes')
    else:
        image = image.crop(cropRect)

    return image

def framecap(config, logger, serdev):
    framenum = get_most_recent_frame(config)
    cap = cv2.VideoCapture(config.camindex)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, res[config.res][0])
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, res[config.res][1])
    for x in range(1,100):
        ret, frame = cap.read()
        filename = "{}/{:>08}.bmp".format(config.framesto, framenum)
        logger.info(f'Saving to {filename}')
        cv2.imwrite(filename, frame)
        cropped = findSprocketsS8(logger, Image.fromarray(frame), draw=True)
        if cropped is not None:
            filename = "{}/{:>08}_cropped.bmp".format(config.framesto, framenum)
            logger.info(f'Saving to {filename}')
            cv2.imwrite(filename, np.asarray(cropped))

        framenum += 1
        time.sleep(1.0)
        serdev.write(b'n')
        wait = serwaitfor(serdev, b'{HDONE}', b'{NTO}', logger)
    cap.release()


def main():
    config = proc_commandline()
    logger = setlogging(config)
    serdev = init(config, logger)
    framecap(config, logger, serdev)

main()
