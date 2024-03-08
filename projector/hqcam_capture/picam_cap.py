#!/usr/bin/env python3
#./opencvcap.py --serialport /dev/ttyACM0 --camindex 0 --film s8 --framesto ~/share/opencvcap0 --startdia 140 --enddia 80 --res draft

import  argparse
#import cProfile
import  cv2 as cv
#from Exscript.util.interact import read_login
#from Exscript.protocols import Telnet
from functools import partial
from    glob import glob, iglob
from    itertools import groupby
import  logging
from    logging.handlers import RotatingFileHandler
import  math
import  numpy as np
import  os
import  pdb
#from    PIL import Image, ImageDraw, ImageFilter, ImageOps
#from    scipy import ndimage
import  serial
import sys
from    Tension import Tension
import  time
import subprocess
import signal

def handler(signum, frame):
    print('Ctrl-C detected, exiting gracefully')
    sys.exit(0)


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

def pcl_framecap():
    parser = argparse.ArgumentParser()
#    parser.add_argument('--camindex', dest='camindex', help='Camera Index (/dev/videoX)', default=0)
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


#def framegen(capture, config, logger, startframe):
#
#    for count in range(config.frames):
#        global lastTension
#        if tension[count+startframe] != lastTension:
#            lastTension = tension[count+startframe]
#            logger.info(f'Tension {lastTension}')
#            serwrite(str(lastTension).encode())
#        serwrite(b'n')
#        wait = serwaitfor(b'{HDONE}', b'{NTO}')
#        if wait[0]:
#            logger.error(wait[2])
#            return
#
#        frames = []
#        for exp in config.exposure.split(','):
#            logger.debug(f'Exposure {cap.get(cv.CAP_PROP_EXPOSURE)}')
#            ret, frame = capture.read(exp=exp)
#            logger.debug(f'ret {ret}')
#            frames.append(frame)
#
#        yield frames,config.exposure.split(',')
#
#    cap.release()

def framecap(config):
    startframe = get_most_recent_frame(config)

    for framenum in range(config.frames):
        global lastTension
        lastTension = tension[framenum+startframe]
        logger.info(f'Tension {lastTension}')
        serwrite(str(lastTension).encode())
        serwrite(b'n')
        wait = serwaitfor(b'{HDONE}', b'{NTO}')
        if wait[0]:
            logger.error(wait[2])
            break

        frames = []
        for exp in config.exposure.split(','):
            try:
                target = f'{config.framesto}/{startframe+framenum:>08}_{exp}.png'
                command=f'rpicam-still -n --hflip=1 --immediate --width=2304 --height=1296 -e png --awb=indoor --shutter {exp} --output {target}'
                logger.debug(command)
                rc = subprocess.run(command.split())
            except Exception as ee:
                logger.error(f'capture failed {str(ee)}')
                break
    serwrite(b' ')


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

def main():
    signal.signal(signal.SIGINT, handler)
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
    else:
        print(f'Unknown command {sys.argv[1]}')
        sys.exit(1)

    if 'exptest' == config.do: exptest(config)
    if 'framecap' == config.do:
        framecap(config)

    if 'tonefuse' == config.do: tonefuse(config)

main()
