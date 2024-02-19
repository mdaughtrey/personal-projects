#!/usr/bin/env python3
#./opencvcap.py --serialport /dev/ttyACM0 --camindex 0 --film s8 --framesto ~/share/opencvcap0 --startdia 140 --enddia 80 --res draft

import  argparse
import cProfile
import  cv2 as cv
from Exscript.util.interact import read_login
from Exscript.protocols import Telnet
from functools import partial
from    glob import glob, iglob
from    itertools import groupby
import  logging
from    logging.handlers import RotatingFileHandler
import  math
import  numpy as np
import  os
import  pdb
from    PIL import Image, ImageDraw, ImageFilter, ImageOps
from    scipy import ndimage
import  serial
import sys
from    Tension import Tension
import  time

# CAP_PROP_AUTOFOCUS: 1.0
# CAP_PROP_AUTO_EXPOSURE: 3.0
# CAP_PROP_AUTO_WB: 1.0
# CAP_PROP_BACKEND: 200.0
# CAP_PROP_BACKLIGHT: 0.0
# CAP_PROP_BRIGHTNESS: 20.0
# CAP_PROP_BUFFERSIZE: 4.0
# CAP_PROP_CONTRAST: 50.0
# CAP_PROP_CONVERT_RGB: 1.0
# CAP_PROP_EXPOSURE: 156.0
# CAP_PROP_FORMAT: 16.0
# CAP_PROP_FOURCC: 1448695129.0
# CAP_PROP_FPS: 30.0
# CAP_PROP_FRAME_HEIGHT: 480.0
# CAP_PROP_FRAME_WIDTH: 640.0
# CAP_PROP_GAIN: 57.0
# CAP_PROP_GAMMA: 300.0
# CAP_PROP_HUE: 0.0
# CAP_PROP_MODE: 0.0
# CAP_PROP_POS_MSEC: 0.0
# CAP_PROP_SATURATION: 68.0
# CAP_PROP_SHARPNESS: 50.0
# CAP_PROP_TEMPERATURE: 4600.0
# CAP_PROP_WB_TEMPERATURE: 4600.0

res = [(640,480),(3264,2448),(2592,1944),(1920,1080),(1600,1200),(1280,720),(960,540),(848,480),(640,360),(424,240),(320,240),(320,180),(640,480)]
serdev = None
steps_per_frame=85
last_delta = 0
capmode='RGB'
logger = None

class SerDev():
    def __init__(self, config):
        self.serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
#        logger = logging.getLogger('serdev')
#        logger.setLevel(logging.DEBUG)
        if self.serdev.isOpen():
            self.serdev.close()
        self.serdev.open()
        self.write(b' ')

    def write(self, message):
        logger.debug(message)
        self.serdev.write(message)

    def waitfor(self, text1, text2):
        accum = b''
        logger.debug("Waiting on %s or %s" % (text1, text2))
        while not text1 in accum and not text2 in accum:
            try:
                accum += self.serdev.read()
            except:
                logger.debug("serwaitfor timeout")
        if text1 in accum:
            logger.debug("Matched on %s" % text1)
            return (0, text1, accum)
        if text2 in accum:
            logger.debug("Matched on %s" % text2)
            return (1, text2, accum)

    def waitready(self):
        return self.waitfor(b'{State:Ready}', b'No')


class Camera():
    res = [(640,480),(3264,2448),(2592,1944),(1920,1080),(1600,1200),(1280,720),(960,540),(848,480),(640,360),(424,240),(320,240),(320,180),(640,480)]
    def __init__(self, *args):
        self.res = {'lo': Camera.res[0], 'hi': Camera.res[2]}
        if args[0] is None: 
            return
        self.config = args[0]
        self.cap = cv.VideoCapture(self.config.camindex)
        self.cap.set(cv.CAP_PROP_CONVERT_RGB, 0.0)
#        logger = logging.getLogger('camera')
        logger.debug('done')

    def __enter__(self, *args):
        logger.debug('done')

    def __exit__(self):
        if self.cap.isOpened():
            self.cap.close()

    def read(self, res = 'last', exp = 0):
        resval = self.res.get(res, None)
        if resval:
            self.cap.set(cv.CAP_PROP_FRAME_WIDTH, resval[0])
            self.cap.set(cv.CAP_PROP_FRAME_HEIGHT, resval[1])
        if exp:
            self.cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
            self.cap.set(cv.CAP_PROP_EXPOSURE, exp)
        else:
            self.cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 3)
        return self.cap.read()

def pcl_framecap():
    parser = argparse.ArgumentParser()
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument(dest='do')
    parser.add_argument('--debug', dest='debug', action='store_true', help='debug (no crop, show lines)')
    parser.add_argument('--showwork', dest='showwork', action='store_true', help='show intermediary images')
    parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
    parser.add_argument('--fastforward', dest='fastforward', type=int, help='fast forward multiplier', default=8)
    parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8', required=True)
    parser.add_argument('--frames', dest='frames', type=int, default=-1, help='Number of frames to capture')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', required=True, help='Log file')
    parser.add_argument('--optocount', dest='optocount', type=int, default=int(steps_per_frame/10), help='optocount per frame')
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    parser.add_argument('--exposure', dest='exposure', help='EDR exposure a,b,[c,..]')
    parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
    parser.add_argument('--usevlc', dest='usevlc', type=str,  help='host:port:pswd')
    return parser.parse_args()

def pcl_exptest():
    parser = argparse.ArgumentParser()
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument(dest='do')
    parser.add_argument('--debug', dest='debug', action='store_true', help='debug (no crop, show lines)')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', required=True, help='Log file')
    parser.add_argument('--nofilm', dest='nofilm', default=False, action='store_true', help="no film")
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    return parser.parse_args()

def pcl_tonefuse():
    parser = argparse.ArgumentParser()
    parser.add_argument(dest='do')
    parser.add_argument('--framesfrom', dest='framesfrom', required=True, help='Source Directory')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', default='usbcap.log', help='Log file')
    return parser.parse_args()

def pcl_oneshot():
    parser = argparse.ArgumentParser()
    parser.add_argument(dest='do')
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', default='usbcap.log', help='Log file')
    parser.add_argument('--serdev', dest='serdev', default='/dev/ttyACM0', help='Serial device')
    parser.add_argument('--exposure', type=int, dest='exposure', help='exposure', default=5000)
    return parser.parse_args()

def setlogging(config):
    global logger
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('usbcap')
    fileHandler = logging.FileHandler(filename = config.logfile)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

def serwrite(message):
    logger.debug(message)
    serdev.write(message)

def serwaitfor(text1, text2):
    accum = b''
#    logger = logging.getLogger('hqcap')
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        try:
            accum += serdev.read()
        except:
            logger.debug("serwaitfor timeout")
        #logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return (0, text1, accum)
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return (1, text2, accum)

def init_framecap(config):
#    pdb.set_trace()
    if config.showwork and not os.path.exists(workdir:='{}/work'.format(config.framesto)):
        os.mkdir(workdir)
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    tension = np.multiply(np.array(tension), 9)
    numframes += 1000
#    logger = logging.getLogger('framecap')
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))

    global lastTension
    lastTension = tension[0]

    logger.info("Opening %s" % config.serdev)
    global serdev
    serdev = serial.Serial(config.serdev, 115200) # , timeout=1)
    if serdev.isOpen():
        serdev.close()
    serdev.open()
    serwrite(b' ')
    wait = serwaitfor(b'{State:Ready}', b'No')
    # k - echo
    # c - clear
    # v - verbose
    # t - tension
    # l - lamp
    # e - encoder limit
    # c - clear param
    # E - encodeer slow threshold 8000
    # o - timeout
    # C - stepper menu
    # - c - clear param
    # - I - min interval 500
    # - c - clear param
    # - i - max interval 2000
    # - c - clear param
    # - D - ramp down 25
    # - U - ramp up 25
    # - x - return to main
    #message = f'kcv{lastTension}tl{config.optocount}ecE8000oCc500Ic2000ic25D25Ux'.encode()          
    message = f'Wcv{lastTension}tl'.encode()
    logger.debug(message)
    serwrite(message)

def init_exptest(config):
    pass

def init_tonefuse(config):
    pass

def init_oneshot(config):
    serial = SerDev(config)
    serial.waitready()
    cap = cv.VideoCapture(config.camindex)
    cap.set(cv.CAP_PROP_FRAME_WIDTH, res[1][0])
    cap.set(cv.CAP_PROP_FRAME_HEIGHT, res[1][1])
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
    cap.set(cv.CAP_PROP_EXPOSURE, config.exposure)
    serial.write(b'l')
    ret, frame = cap.read()
    serial.write(b' ')
    if not ret:
        logger.error(f'cap.read {count} failed')
        sys.exit(1)
    cv.imwrite(f'{config.framesto}/oneshot.png', np.asarray(frame))
    cap.release()

def oneshot(config):
    pass

def get_most_recent_frame(config):
    files = sorted(glob("{0}/????????.png".format(config.framesto)))
    if len(files):
        frames =  [int(os.path.basename(os.path.splitext(f)[0])) for f in files]
        frame = 1+ sorted(frames)[::-1][0]
    else:
        files = sorted(glob("{0}/????????_*.png".format(config.framesto)))
        if len(files):
            return max([int(os.path.basename(f).split('_')[0]) for f in files])
        else:
            return 0
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

def findSprocketsS8(image, threshtest = True):
#    logger = logging.getLogger('hqcap')
    grey = image.convert('L')
    flattened = np.asarray(grey, dtype=np.uint8)
    eroded = ndimage.grey_erosion(flattened, size=(5,5))
    sliceL = 0
    sliceR = int(image.size[0]/40)
    slice = eroded[:,sliceL:sliceR]
    if config.showwork: dw('eroded', slice)
    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)
#    pdb.set_trace()

    #threshold=205
    threshold = int(np.average([darkest, lightest]))
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 1
    if config.showwork: 
        sl2 = np.copy(slice)
        sl2[sl2 == 1] = 255
        dw('bw', sl2)

    rect = findExtents(slice)
    yCenter = rect[1] + int((rect[3] - rect[1])/2)
    delta = int(abs(yCenter-image.size[1]/2))
    threshold =  int(image.size[1]/20)
    logger.debug(f'yCenter {yCenter} delta {delta} threshold {threshold}')

    if config.showwork:
        draw = ImageDraw.Draw(image)
        for line in [(0, image.size[1]/2, image.size[0]/2, image.size[1]/2),
            (0, image.size[1]/2-threshold, image.size[0]/2, image.size[1]/2-threshold),
            (0, image.size[1]/2+threshold, image.size[0]/2, image.size[1]/2+threshold),
            (0, yCenter, image.size[0]-1, yCenter)]:
            draw.line(line, fill=255, width=2)

    # Reject if above threshold
    if delta > threshold and True == threshtest:
        if config.showwork:
            return (image,None)
        else:
            logger.debug('rejecting frame')
            return (None,None)

    xCenter = int(image.size[1]/2)
    (ix, iy) = image.size
    xCenter = int(iy/2)
    yofs = xCenter - yCenter
    # Find right extent of sprocket hole
#    if False == threshtest:
    if True:
        sp=np.copy(flattened[yCenter-10:yCenter+10,:450])
        sp[sp < threshold] = 0
        sp[sp >= threshold] = 1
        if config.showwork:
            dw('sp',sp)
#        xx = findExtents(sp)

    # x0, y0, x1, y1
    cropRect = (0, int(iy/20) - yofs, ix, int(iy*.95)-yofs)
    logger.debug(f'yofs {yofs} rect {cropRect[0]} {cropRect[1]} {cropRect[2]} {cropRect[3]}')

    if config.showwork:
        def tupleAdd(t0, t1):
            return tuple(map(sum, (zip(t0, t1))))
        draw = ImageDraw.Draw(image)
        draw.rectangle(cropRect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(cropRect, (1,1,-1,-1)), outline='#ffffff', width=1)
        return (image,cropRect)

    return (None,cropRect)
#    else:
#        image = image.crop(cropRect)
#    return image
    

def dw(name, data):
#    logger = logging.getLogger('hqcap')
    fname = f'{config.framesto}/work/{name}_{time.time()}.png'
    logger.debug(f'Writing to {fname}')
    cv.imwrite(f'{fname}.png', np.asarray(data))


def findSprockets8mm(image, dbg = False):
#    logger = logging.getLogger('hqcap')
    grey = image.convert('L')
#    pdb.set_trace()
    flattened = np.asarray(grey, dtype=np.uint8)
    eroded = ndimage.grey_erosion(flattened, size=(5,5))
    sliceL = 0
    sliceR = int(image.size[0]/40)
    slice = eroded[:,sliceL:sliceR]
    # get the darkest and lightest values, their midpoint is the threshold
    darkest = ndimage.minimum(slice)
    lightest = ndimage.maximum(slice)

#    threshold=205
    threshold = int(np.average([darkest, lightest]))
    slice[slice < threshold] = 0
    slice[slice >= threshold] = 1
    slice ^= 1

    rect = findExtents(slice)
    yCenter = rect[1] + (rect[3] - rect[1])/2
    delta = abs(yCenter-image.size[1]/2)
    threshold =  int(image.size[1]/20)
    logger.debug(f'yCenter {yCenter} delta {delta} threshold {threshold}')

    if dbg:
        draw = ImageDraw.Draw(image)
        draw.line((0, image.size[1]/2, image.size[0]/2, image.size[1]/2), fill='#29e8e8', width=2)
        draw.line((0, image.size[1]/2-threshold, image.size[0]/2, image.size[1]/2-threshold), fill='#29e8e8', width=2)
        draw.line((0, image.size[1]/2+threshold, image.size[0]/2, image.size[1]/2+threshold), fill='#29e8e8', width=2)
        draw.line((0, yCenter, image.size[0]-1, yCenter), fill='#ff0000', width=2)

    # Reject if above threshold
    if delta > threshold:
        logger.debug('rejecting frame')
        return None

#    pdb.set_trace()
    xCenter = image.size[1]/2
    yofs = image.size[1]/2 - yCenter
    (ix, iy) = image.size
    # x0, y0, x1, y1
    cropRect = (0, int(iy/20 - yofs), ix, int(iy*.95-yofs))
    #cropRect = (0, int(iy/10 - yofs), ix, int(iy*.90-yofs))
    logger.debug(f'yofs {yofs} rect {cropRect[0]} {cropRect[1]} {cropRect[2]} {cropRect[3]}')

    if dbg:
        def tupleAdd(t0, t1):
            return tuple(map(sum, (zip(t0, t1))))
        draw = ImageDraw.Draw(image)
        draw.rectangle(cropRect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(cropRect, (1,1,-1,-1)), outline='#ffffff', width=1)
        return image
    else:
        image = image.crop(cropRect)

    return image


def write_frame(config, framenum, frame):
#    logger = logging.getLogger('hqcap')
    filename = f'{config.framesto}/{framenum:>08}.png'
    logger.info(f'Saving to {filename}')
    try:
        cv.imwrite(filename, np.asarray(frame))
    except:
        logger.error("cv rejects frame")
#        pdb.set_trace()
#        open(f'{config.framesto}/rejected_{framenum:>08}','wb').write(frame)


def framegen(config, logger):
#    def setres(cap, i):
#        if cap is None or not cap.isOpened():
#            cap = cv.VideoCapture(config.camindex)
#        cap.set(cv.CAP_PROP_FRAME_WIDTH, res[i][0])
#        cap.set(cv.CAP_PROP_FRAME_HEIGHT, res[i][1])
#        cap.set(cv.CAP_PROP_EXPOSURE, 1200)
#        return cap

#    setres(cap,2)
#    finder = {'super8':findSprocketsS8, '8mm': findSprockets8mm}[config.film]
#    stepsPerFrame = config.optocount * config.fastforward
#    cap = setres(None, 0)
    cap = cv.VideoCapture(config.camindex)
    count = 0
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
    while count < config.frames:
#        cap = setres(cap, 0)
        ret, frame = cap.read()
        if not ret:
            logger.error(f'cap.read {count} failed')
            sys.exit(1)

        image = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
        serwrite(f'n'.encode())
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            return
        yield image


#        (cropped,rect) = finder(image)
#        if cropped is not None:
#            cap = setres(cap, 1)
#            ret, frame = cap.read()
#            # BGR to RGB
#            frame.T[[0,2]]=frame.T[[2,0]]
#            image = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
#
#            (cropped,rect) = finder(image, threshtest = False)
#            div = np.array(2*[cropped.size[0] / 2160])
#            cropped = cropped.resize(np.array(np.array(cropped.size, np.float64)/div,
#                np.uint32), Image.Resampling.LANCZOS)
#            if cropped.size[1] % 2:
#                cropped = cropped.crop((0, 0, cropped.size[0], cropped.size[1] - 1))
#            
#            count += 1
#            serwrite(f'c{stepsPerFrame}emn'.encode())
#            yield cropped
#        else:
#            serwrite(f'c{config.optocount}emn'.encode())
#        wait = serwaitfor(b'{HDONE}', b'{NTO}')
#        if wait[0]:
#            logger.error(wait[2])
#            return
    cap.release()

def framegen_detect(config, logger, startframe):
    def setres(cap, i):
        if cap is None or not cap.isOpened():
            cap = cv.VideoCapture(config.camindex)
        cap.set(cv.CAP_PROP_FRAME_WIDTH, res[i][0])
        cap.set(cv.CAP_PROP_FRAME_HEIGHT, res[i][1])
        cap.set(cv.CAP_PROP_EXPOSURE, 1200)
        return cap

    finder = {'super8':findSprocketsS8, '8mm': findSprockets8mm}[config.film]
    stepsPerFrame = config.optocount * config.fastforward
    cap = setres(None, 0)
    count = 0
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
    while count < config.frames:
        if tension[count+startframe] != lastTension:
            lastTension = tension[count+startframe]
            logger.info(f'Tension {lastTension}')
            serwrite(f'{lastTension}')

        cap = setres(cap, 0)
        ret, frame = cap.read()
        if not ret:
            logger.error(f'cap.read {count} failed')
            sys.exit(1)

        image = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
        (cropped,rect) = finder(image)
        if cropped is not None:
            cap = setres(cap, 1)
            ret, frame = cap.read()
            # BGR to RGB
            frame.T[[0,2]]=frame.T[[2,0]]
            image = ImageOps.mirror(Image.fromarray(frame,mode=capmode))

            (cropped,rect) = finder(image, threshtest = False)
            div = np.array(2*[cropped.size[0] / 2160])
            cropped = cropped.resize(np.array(np.array(cropped.size, np.float64)/div,
                np.uint32), Image.Resampling.LANCZOS)
            if cropped.size[1] % 2:
                cropped = cropped.crop((0, 0, cropped.size[0], cropped.size[1] - 1))
            
            count += 1
            serwrite(f'c{stepsPerFrame}emn'.encode())
            yield cropped
        else:
            serwrite(f'c{config.optocount}emn'.encode())
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            return
    cap.release()

def framegen_edr_detect(config, logger, startframe):
    def setres(cap, i):
        cap.set(cv.CAP_PROP_FRAME_WIDTH, res[i][0])
        cap.set(cv.CAP_PROP_FRAME_HEIGHT, res[i][1])

    cap = cv.VideoCapture(config.camindex)
    finder = {'super8':findSprocketsS8, '8mm': findSprockets8mm}[config.film]
    stepsPerFrame = config.optocount * config.fastforward
    setres(cap, 0)
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)
    count = 0
    while count < config.frames:
        if tension[count+startframe] != lastTension:
            lastTension = tension[count+startframe]
            logger.info(f'Tension {lastTension}')
            serwrite(f'{lastTension}')

        setres(cap, 0)
        ret, frame = cap.read()
        if not ret:
            logger.error(f'cap.read {count} failed')
            sys.exit(1)
        frame = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
        (cropped,croprect) = finder(frame)
        if croprect is None:
            serwrite(f'c{config.optocount}emn'.encode())
        else:
            croprect = None
            frames = []
            for exp in config.exposure.split(','):
                setres(cap, 1)
                cap.set(cv.CAP_PROP_EXPOSURE, int(exp))
                logger.debug(f'Exposure {cap.get(cv.CAP_PROP_EXPOSURE)}')
                ret, frame = cap.read()
                # BGR to RGB
                frame.T[[0,2]]=frame.T[[2,0]]
                logger.debug(f'ret {ret}')
                frame = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
                if croprect is None:
                    (_,croprect) = finder(frame, threshtest = False)
                frame = frame.crop(croprect) 
                div = np.array(2*[frame.size[0] / 2160])
                frame = frame.resize(np.array(np.array(frame.size, np.float64)/div,
                    np.uint32), Image.Resampling.LANCZOS)
                if frame.size[1] % 2:
                    frame = frame.crop((0, 0, frame.size[0], frame.size[1] - 1))
                frames.append(frame)
            
            count += 1
            serwrite(f'c{stepsPerFrame}emn'.encode())
            yield frames,config.exposure.split(',')
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            return
    cap.release()

def framegen_edr(config, logger, startframe):
    def setres(cap, i):
        cap.set(cv.CAP_PROP_FRAME_WIDTH, res[i][0])
        cap.set(cv.CAP_PROP_FRAME_HEIGHT, res[i][1])

    cap = cv.VideoCapture(config.camindex)
    setres(cap, 2)
    cap.set(cv.CAP_PROP_AUTO_EXPOSURE, 1)

#    count = 0
#    while count < config.frames:
    for count in range(config.frames):
        global lastTension
        if tension[count+startframe] != lastTension:
            lastTension = tension[count+startframe]
            logger.info(f'Tension {lastTension}')
            serwrite(str(lastTension).encode())
        serwrite(b'n')
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            return

        frames = []
        for exp in config.exposure.split(','):
            cap.set(cv.CAP_PROP_EXPOSURE, int(exp))
            logger.debug(f'Exposure {cap.get(cv.CAP_PROP_EXPOSURE)}')
            time.sleep(3.5)
            ret, frame = cap.read()
            # BGR to RGB
            frame.T[[0,2]]=frame.T[[2,0]]
            logger.debug(f'ret {ret}')
            frame = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
            frames.append(frame)

        yield frames,config.exposure.split(',')

    cap.release()

#        ret, frame = cap.read()
#        if not ret:
#            logger.error(f'cap.read {count} failed')
#            sys.exit(1)
#        frame = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
#
#
#        (cropped,croprect) = finder(frame)
#        if croprect is None:
#            serwrite(f'c{config.optocount}emn'.encode())
#        else:
#            croprect = None
#            frames = []
#            for exp in config.exposure.split(','):
#                setres(cap, 1)
#                cap.set(cv.CAP_PROP_EXPOSURE, int(exp))
#                logger.debug(f'Exposure {cap.get(cv.CAP_PROP_EXPOSURE)}')
#                ret, frame = cap.read()
#                # BGR to RGB
#                frame.T[[0,2]]=frame.T[[2,0]]
#                logger.debug(f'ret {ret}')
#                frame = ImageOps.mirror(Image.fromarray(frame,mode=capmode))
#                if croprect is None:
#                    (_,croprect) = finder(frame, threshtest = False)
#                frame = frame.crop(croprect) 
#                div = np.array(2*[frame.size[0] / 2160])
#                frame = frame.resize(np.array(np.array(frame.size, np.float64)/div,
#                    np.uint32), Image.Resampling.LANCZOS)
#                if frame.size[1] % 2:
#                    frame = frame.crop((0, 0, frame.size[0], frame.size[1] - 1))
#                frames.append(frame)
#            
#            count += 1
#            serwrite(f'c{stepsPerFrame}emn'.encode())
#            yield frames,config.exposure.split(',')
#        wait = serwaitfor(b'{HDONE}', b'{NTO}')
#        if wait[0]:
#            logger.error(wait[2])
#            return
#    cap.release()

def exptest(config):
#    logger = logging.getLogger('exptest')
    serial = SerDev(config)
    serial.waitready()
    serial.write(b'30tl')

    camera = Camera(config)
    for exp in range(100, 20000, 100 ):
        logger.debug(f'Exposure {exp}')
        ret, frame = camera.read(res=config.res, exp=exp)
        if False == ret:
            logger.error(f'Exposure {exp} read fails {ret}')
        else:
            write_frame(config, exp, frame)


def framecap(config):
#    logger = logging.getLogger('hqcap')
    framenum = get_most_recent_frame(config)

    if config.exposure:
        for frames, exposures in framegen_edr(config, logger,framenum):
            [f.save(f'{config.framesto}/{framenum:>08}_{e}.png') for f,e in zip(frames,exposures)]
            logger.info(f'Saving to {config.framesto}/{framenum}_*.png')
            framenum += 1
    else:
        for index,frame in enumerate(framegen(config, logger)):
            write_frame(config, framenum+index, frame)

#    serwrite(b'M')
    serwrite(b' ')
    wait = serwaitfor(b'{State:Ready}', b'No')
    logger.debug(wait[2])
#    serwrite(b' ')

def framecap_vlc(config):
#    logger = logging.getLogger('framcap')
    count = 0
    # Start telnet
    params = config.telnet.split(":")
    conn = Telnet()
    conn.connect('localhost',4212)
    conn.waitfor(conn.get_password_prompt())
    conn.send('abc\r\n')

    while count < config.frames:
        # telnet
        conn.send('snapshot\r\n')
        time.sleep(0.1)
        serwrite(f'n'.encode())
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        time.sleep(1.0)
        if wait[0]:
            logger.error(wait[2])
            return


def tonefuse(config):
    images = [cv.imread(f'{config.framesfrom}/00000000a.png'),
        cv.imread(f'{config.framesfrom}/00000000b.png')]
    times = np.asarray([.700, 1.4], dtype=np.float32)

    calibrate = cv.createCalibrateDebevec()
    response = calibrate.process(images, times)
    merge_debevec = cv.createMergeDebevec()
    hdr = merge_debevec.process(images, times, response)
    tonemap = cv.createTonemap(2.2)
    ldr = tonemap.process(hdr)
    merge_mertens = cv.createMergeMertens()
    fusion = merge_mertens.process(images)

    cv.imwrite(f'{config.framesto}/fusion.png', fusion * 255)
    cv.imwrite(f'{config.framesto}/ldr.png', ldr * 255)
    cv.imwrite(f'{config.framesto}/hdr.png', hdr * 255)

    pass

def getres(config):
    cap = cv.VideoCapture(config.camindex)
    for r in res:
        cap.set(cv.CAP_PROP_FRAME_WIDTH, r[0])
        cap.set(cv.CAP_PROP_FRAME_HEIGHT, r[1])
        print('{} x {}'.format(cap.get(cv.CAP_PROP_FRAME_WIDTH),cap.get(cv.CAP_PROP_FRAME_HEIGHT)))

    cap.release()
    sys.exit(0)

def main():
    global config
    if 'framecap' == sys.argv[1]:
        config = pcl_framecap()
        setlogging(config)
        init_framecap(config)
    elif 'exptest' == sys.argv[1]:
        config = pcl_exptest()
        setlogging(config)
        init_exptest(config)
    elif 'tonefuse' == sys.argv[1]:
        config = pcl_tonefuse()
        setlogging(config)
        init_tonefuse(config)
    elif 'oneshot' == sys.argv[1]:
        config = pcl_oneshot()
        setlogging(config)
        init_oneshot(config)
    else:
        print(f'Unknown command {sys.argv[1]}')
        sys.exit(1)

    if 'exptest' == config.do: exptest(config)
    if 'framecap' == config.do:
        if config.usevlc:
            framecap_vlc(config)
        else:
            framecap(config)

    if 'tonefuse' == config.do: tonefuse(config)
    if 'oneshot' == config.do: oneshot(config)

main()
