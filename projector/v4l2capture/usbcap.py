#!/usr/bin/env python3
#./opencvcap.py --serialport /dev/ttyACM0 --camindex 0 --film s8 --framesto ~/share/opencvcap0 --startdia 140 --enddia 80 --res draft

import  argparse
import cProfile
import  cv2
from functools import partial
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
    return parser.parse_args()

def pcl_exptest():
    parser = argparse.ArgumentParser()
    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
    parser.add_argument(dest='do')
    parser.add_argument('--debug', dest='debug', action='store_true', help='debug (no crop, show lines)')
    parser.add_argument('--framesto', dest='framesto', required=True, help='Target Directory')
    parser.add_argument('--logfile', dest='logfile', required=True, help='Log file')
    parser.add_argument('--res', dest='res', type=int, choices=range(len(res)), default=0, help='resolution select [0-{}]'.format(len(res)-1))
    return parser.parse_args()

def setlogging(config):
    FormatString='%(asctime)s %(levelname)s %(funcName)s %(lineno)s %(message)s'
    logging.basicConfig(level = logging.DEBUG, format=FormatString)
    logger = logging.getLogger('hqcap')
    fileHandler = logging.FileHandler(filename = config.logfile)
    fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
    fileHandler.setLevel(logging.DEBUG)
    logger.addHandler(fileHandler)

def serwrite(message):
    logging.getLogger('hqcap').debug(message)
    serdev.write(message)

def serwaitfor(text1, text2):
    accum = b''
    logger = logging.getLogger('hqcap')
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
    if config.showwork and not os.path.exists(workdir:='{}/work'.format(config.framesto)):
        os.mkdir(workdir)
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    tension = np.multiply(np.array(tension), 9)
    numframes += 1000
    logger = logging.getLogger('hqcap')
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
    message = f'kcv{lastTension}tl{config.optocount}ecE8000oCc500Ic2000ic25D25Ux'.encode()          
    logger.info(message)
    serwrite(message)

def init_exptest(config):
    pass

def get_most_recent_frame(config):
    files = sorted(glob("{0}/????????.png".format(config.framesto)))
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

def findSprocketsS8(image, threshtest = True):
    logger = logging.getLogger('hqcap')
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
            return image
        else:
            logger.debug('rejecting frame')
            return None

    xCenter = int(image.size[1]/2)
    (ix, iy) = image.size
    xCenter = int(iy/2)
    yofs = xCenter - yCenter
    # x0, y0, x1, y1
    cropRect = (0, int(iy/20) - yofs, ix, int(iy*.95)-yofs)
    logger.debug(f'yofs {yofs} rect {cropRect[0]} {cropRect[1]} {cropRect[2]} {cropRect[3]}')

    if config.showwork:
        def tupleAdd(t0, t1):
            return tuple(map(sum, (zip(t0, t1))))
        draw = ImageDraw.Draw(image)
        draw.rectangle(cropRect, outline='#ff0000', width=1)
        draw.rectangle(tupleAdd(cropRect, (1,1,-1,-1)), outline='#ffffff', width=1)
        return image
    else:
        image = image.crop(cropRect)

    return image

def dw(name, data):
    logger = logging.getLogger('hqcap')
    fname = f'{config.framesto}/work/{name}_{time.time()}.png'
    logger.debug(f'Writing to {fname}')
    cv2.imwrite(f'{fname}.png', np.asarray(data))


def findSprockets8mm(image, dbg = False):
    logger = logging.getLogger('hqcap')
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
    logger = logging.getLogger('hqcap')
    filename = f'{config.framesto}/{framenum:>08}.png'
    logger.info(f'Saving to {filename}')
    try:
        cv2.imwrite(filename, np.asarray(frame))
    except:
        logger.error("cv2 rejects frame")
#        pdb.set_trace()
#        open(f'{config.framesto}/rejected_{framenum:>08}','wb').write(frame)


def framegen(config, logger):
    def setres(cap, i):
        if cap is None or not cap.isOpened():
            cap = cv2.VideoCapture(config.camindex)
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, res[i][0])
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, res[i][1])
        return cap

    finder = {'super8':findSprocketsS8, '8mm': findSprockets8mm}[config.film]
    stepsPerFrame = config.optocount * config.fastforward
    cap = setres(None, 0)
    count = 0
    while count < config.frames:
        cap = setres(cap, 0)
        ret, frame = cap.read()
        if not ret:
            logger.error(f'cap.read {count} failed')
            sys.exit(1)
        cropped = finder(Image.fromarray(frame))
        if cropped is not None:
            cap = setres(cap, 1)
            ret, frame = cap.read()
            cropped = finder(Image.fromarray(frame), threshtest = False)
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

def framegen_edr(config, logger):
    def setres(cap, i):
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, res[i][0])
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, res[i][1])

    cap = cv2.VideoCapture(config.camindex)
    finder = {'super8':findSprocketsS8, '8mm': findSprockets8mm}[config.film]
    stepsPerFrame = config.optocount * config.fastforward
    setres(cap, 0)
    cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)
    count = 0
    while count < config.frames:
        setres(cap, 0)
        ret, frame = cap.read()
        if not ret:
            logger.error(f'cap.read {count} failed')
            sys.exit(1)
        cropped = finder(Image.fromarray(frame))
        if cropped is not None:
            frames = []
            for exp in config.exposure.split(','):
                setres(cap, 1)
                cap.set(cv2.CAP_PROP_EXPOSURE, int(exp))
                logger.debug(f'Exposure {cap.get(cv2.CAP_PROP_EXPOSURE)}')
                ret, frame = cap.read()
                logger.debug(f'ret {ret}')
                cropped = finder(Image.fromarray(frame), threshtest = False)
                div = np.array(2*[cropped.size[0] / 2160])
                cropped = cropped.resize(np.array(np.array(cropped.size, np.float64)/div,
                    np.uint32), Image.Resampling.LANCZOS)
                if cropped.size[1] % 2:
                    cropped = cropped.crop((0, 0, cropped.size[0], cropped.size[1] - 1))
                frames.append(cropped)
            
            count += 1
            serwrite(f'c{stepsPerFrame}emn'.encode())
            yield frames
        else:
            serwrite(f'c{config.optocount}emn'.encode())
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            return
    cap.release()

def exptest(config):
    logger = logging.getLogger('hqcap')
    cap = cv2.VideoCapture(config.camindex)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)
    logger.debug(f'CAP_PROP_AUTO_EXPOSURE {cap.get(cv2.CAP_PROP_AUTO_EXPOSURE)}')

    for exp in range(50, 2000, 50):
        logger.debug(f'Exposure {exp}')
        cap.set(cv2.CAP_PROP_EXPOSURE, exp)
        logger.debug(cap.get(cv2.CAP_PROP_EXPOSURE))
        ret, frame = cap.read()
        if False == ret:
            logger.error(f'Exposure {exp} read fails {ret}')
        else:
            write_frame(config, exp, frame)


def framecap(config):
    logger = logging.getLogger('hqcap')
    framenum = get_most_recent_frame(config)

    if config.exposure:
        for index, frame in enumerate(framegen_edr(config, logger)):
            fname = f'{config.framesto}/{framenum+index:>08}'
            logger.info(f'Saving to {fname}')
            try:
                cv2.imwrite(f'{fname}a.png', np.asarray(frame[0]))
                cv2.imwrite(f'{fname}b.png', np.asarray(frame[1]))
            except:
                logger.error("cv2 rejects frame")
    else:
        for index,frame in enumerate(framegen(config, logger)):
            write_frame(config, framenum+index, frame)

#    serwrite(b'M')
    serwrite(b' ')
    wait = serwaitfor(b'{State:Ready}', b'No')
    logger.debug(wait[2])
#    serwrite(b' ')

def getres(config):
    cap = cv2.VideoCapture(config.camindex)
    for r in res:
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, r[0])
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, r[1])
        print('{} x {}'.format(cap.get(cv2.CAP_PROP_FRAME_WIDTH),cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))

    cap.release()
    sys.exit(0)

def main():
    global config
    logger = logging.getLogger('usbcap')
    if 'framecap' == sys.argv[1]:
        config = pcl_framecap()
        init_framecap(config)
    elif 'exptest' == sys.argv[1]:
        config = pcl_exptest()
        init_exptest(config)
    else:
        print(f'Unknown command {sys.argv[1]}')
        sys.exit(1)

    setlogging(config)
    if 'exptest' == config.do: exptest(config)
    if 'framecap' == config.do: framecap(config)

main()
