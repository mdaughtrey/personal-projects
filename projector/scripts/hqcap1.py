#!/usr/bin/python3

# Sensor modes
# https://www.raspberrypi.org/documentation/raspbian/applications/camera.md
# screen init l 33t 85e

import argparse
from glob import glob, iglob
import logging
from logging.handlers import RotatingFileHandler
import os
import pdb
import re
import serial
import signal
import subprocess
import sys
import threading
import time
import math
#import multiprocessing
#import psutil
from Tension import Tension

from picamera import PiCamera
import warnings
warnings.filterwarnings('default',category=DeprecationWarning)

#BIN='/usr/local/bin/raspiraw'
BIN="raspistill"
SPOOLTYPE="SMALL"
#FILMTYPE="SUPER8"
PREFRAMES=1
FRAMES=0
#SHUTTER=[40000,80000,1200000]

#SHUTTER=[150, 180, 220]
SHUTTER=[100, 150, 180]
TARGETDIR='/tmp'
MAXINFLIGHT=30
SerialPort="/dev/ttyACM0"
OUTPUTDIR="/mnt/exthd"
#OUTPUTDIR="/home/mattd/capture"
port = 0
tension = list()
lastTension = 0
numframes = 0
captureProc = None

intervals = []
lastInterval = 0

Geometry = {'geo0':' --mode 0', 
    'geo1':' --left 804 --top 225 --mode 0', 
    'geo2':' --mode 1', 
    'geo3':' --mode 2', 
    'geo4':' --mode 3', 
    'geo5':' --mode 4', 
    'geo6':' --mode 5', 
    'geo7':' --mode 6', 
    'geo8':' --mode 7', 
    'geo9':' --mode 6 --width 1250', 
}

FormatString='%(asctime)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('hqcap')
fileHandler = logging.FileHandler(filename = './hqcap.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--frames', dest='frames', type=int, default=1e6, help='frames to capture')
parser.add_argument('--dev', dest='dev', help='image device')
parser.add_argument('--prefix', dest='prefix', default='', help='prefix filenames with a prefix')
parser.add_argument('--nofilm', dest='nofilm', action='store_true', default=False, help='run with no film loaded')
parser.add_argument('--noled', dest='noled', action='store_true', default=False, help='run with no LED')
parser.add_argument('--film', dest='film', choices=['super8','8mm'], help='8mm/super8')
parser.add_argument('--dir', dest='dir', required=True, help='set project directory')
parser.add_argument('--single', dest='singleframe', action='store_true', default=False, help='One image per frame')
parser.add_argument('--startdia', dest='startdia', type=int, default=62, help='Feed spool starting diameter (mm)')
parser.add_argument('--enddia', dest='enddia', type=int, default=35, help='Feed spool ending diameter (mm)')
parser.add_argument('--raspid', dest='raspid', type=int, default=0, help='raspistill PID')
parser.add_argument('--picamera', dest='picamera', action='store_true', default=False, help='use picamera lib')
parser.add_argument('--picameracont', dest='picameracont', action='store_true', default=False, help='use picamera lib')
parser.add_argument('--frameinterval', dest='frameinterval', type=int, default=45, help='Frame Interval')
parser.add_argument('--nocam', dest='nocam', action='store_true', default=False, help='no camera operations')
parser.add_argument('--intervals', dest='intervals', help='n,n,n...', required=True)
parser.add_argument('--res', dest='res', choices=['draft','1k', 'hd', 'full'], help="resolution")
config = parser.parse_args()

if config.picamera or config.picameracont and 0 == config.nocam:
    camera = PiCamera()
    res = {'draft':(640, 480), '1k':(1024,768), 'hd':(1920, 1080), 'full':(4056, 3040)}
    camera.resolution = res[config.res]
    camera.shutter_speed = 700 # 700us


def signal_handler(signal, frame):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    port.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)


#def portWaitFor(port, text):
#    accum = b''
#    logger.debug("Waiting on %s" % text)
#    while not text in accum:
#        accum += port.read()
#        #logger.debug("Accumulating %s" % accum)
#    logger.debug("Received %s" % text)
#    return accum

def portWaitFor(port, text):
    accum = b''
    if isinstance(text, bytes):
        text = [text]

    logger.debug("Waiting on %s" % str(text))

    res = [False]
    
    while True not in res:
        accum += port.read()
#        logger.debug(f'Accumulated {accum}')
        res = [t in accum for t in text]
#        logger.debug(f'res {res}')

    logger.debug(f'portWaitFor {accum}')
    logger.debug(f'portWaitFor {res.index(True)}')
    return text[res.index(True)]

#def portWaitFor2(port, text1, text2):
#    accum = b''
#    logger.debug("Waiting on %s or %s" % (text1, text2))
#    while not text1 in accum and not text2 in accum:
#        accum += port.read()
#        #logger.debug("Accumulating %s" % accum)
#    if text1 in accum:
#        logger.debug("Matched on %s" % text1)
#        logger.debug(f'Accumulated {accum}')
#        return text1
#    if text2 in accum:
#        logger.debug("Matched on %s" % text2)
#        logger.debug(f'Accumulated {accum}')
#        return text2

#def captureProc(framenum):
#    outfile = "{}".format("{:s}/{:s}{:06d}a.png".format(config.dir, config.prefix, framenum))
#    args = ['/usr/bin/raspistill','-v',
#        '-e','png',
#        '-q','100',
#        '-t','0',
#        '-s','-fs','0',
#        '--nopreview',
#        '--exposure','night',
#        '--awb','off',
#        '--shutter','10000',
#        '-o','outfile']
#    logger.debug("starting raspistill {}".format(' '.join(args)))
#    proc = subprocess.run(args, capture_output=False)
#    logger.debug("raspistill ends, rc {}".format(task.returncode))
#    return proc

def runCaptureProc(framenum):
#    outfile = "{}".format("{:s}/%06da.png".format(config.dir))
    outfile = "{:s}/%06da.png".format(config.dir)
    #outfile = "{}".format("{:s}/{:s}{:06d}a.png".format(config.dir, config.prefix, framenum))
#    outdir = "{}".format("{:s}/{:s}{:06d}a.png".format(config.dir, config.prefix, framenum))
    capargs = ['/usr/bin/raspistill','-v',
        '-e','png',
        '-q','100',
        '-t','0',
        '-s',
        '-fs', str(framenum),
        '--exposure','night',
        '--awb','backlight',
        '--shutter','1000',
        '--width', '2028',
        '--height', '1520',
        '-o', outfile]
    logger.debug("starting raspistill {}".format(' '.join(capargs)))
    global captureProc
    logger.debug("runCaptureProc: running")
    #captureProc = subprocess.run(args, capture_output=True)
    captureProc = subprocess.Popen(capargs, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=config.dir)
    logger.debug("runCaptureProc: run started")
    #logger.debug("runCaptureProc: run started {}".format(captureProc.stderr.read()))

#def captureWaitFor(proc, text):
#    accum = b''
#    logger.debug("Waiting on %s" % text)
#    while not text in accum:
#        (out, err) = proc.communicate(timeout=1)
#        accum += err
#        logger.debug("Accumulating %s" % accum)
#    logger.debug("Received %s" % text)
#    return accum

def captureWaitFor2(proc, text1, text2):
    accum = b''
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        try:
#            (out, err) = proc.communicate(timeout=1)
            accum += proc.stderr.read1()
        except:
            logger.debug("captureWaitFor timeout")
        logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return (0, text1)
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return (1, text2)

def init(framenum, interval):
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    numframes += 1000
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))
    map (lambda x: x * 3, tension)

    global lastTension
    lastTension = tension[0]

    # start raspistill
    if 0 == config.raspid and 0 == config.nocam:
        if config.picamera or config.picameracont:
            pass
        else:
            thread = threading.Thread(target = runCaptureProc, args = [framenum])
            #runCaptureProc(framenum)
            thread.daemon = True
            thread.start()
            time.sleep(3)
            while captureProc is None:
                logger.debug("Waiting for captureProc")
                time.sleep(1)

            (which, text) = captureWaitFor2(captureProc, 
                b'Waiting for SIGUSR1 to initiate capture and continue or SIGUSR2 to capture and exit',
                b'Failed to run camera app')

            logger.debug("Which {} text {}".format(which, text))
            if which == 1:
                logger.error("captureProc did not initialize [{}], exiting".format(text))
                sys.exit(0)


#        global capture
#        capture = multiprocessing.Process(target = captureProc, args = [framenum])
#        capture.start()
#
#        children = psutil.Process(capture.pid)
#        for proc in children:
#            logger.debug("child PID is {}".format())

    serPort = serial.Serial(SerialPort, 115200) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
#$    serPort.write(b' ')
#    if b'{State:Ready}' != portWaitFor(serPort, b'{State:Ready}'):
#        logger.debug("Init failed")
#        sys.exit(1)

    serPort.write(str.encode(f'{lastTension}tl1d2D{interval}e0E6000o'))
#    portWaitFor(serPort, b'{State:Ready}')
#    if False == config.noled:
#        serPort.write(b'l')
#    serPort.write(b'c%st' % {'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]) 
#    serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film], tension[0]).encode('utf-8'))
    #serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film].encode('utf-8'), tension[0]))
#    portWaitFor(serPort, b'{pt:')
    return serPort

def stop(port):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    logger.debug("Closing")
    port.close()

def frame(port, num, interval):
    global lastTension
    try:
        if (tension[num] != lastTension) & (num < (len(tension) - 1)):
            lastTension = tension[num]
            port.write('-{}t'.format(tension[num]*3).encode('utf-8'))
            logger.debug("Set tension {}".format(lastTension))
    except:
        pass

    if False == config.nofilm:
        port.write(b'?')
        portWaitFor(port, [b'End Config'])

        global lastInterval
        if interval < lastInterval:
            msg = f'{interval}emn'
        else:
            msg = f'{interval}en'
        port.write(str.encode(msg))
        logger.debug(f'Serial write {msg}')
        if b'{NTO}' == portWaitFor(port, [b'{HDONE}', b'{NTO}']):
            logger.error("Frame init timed out, exiting")
            port.write(b' ')
            sys.exit(0)

#    if config.singleframe:
#        ii = zip([SHUTTER[1]], ['a'])
#    else:
#        ii = zip(SHUTTER, ['a','b','c'])
#
#    logger.debug("40")

    if config.nocam:
        time.sleep(2)
        logger.debug("nocam click")
        return 0

    if config.picameracont and 0 == config.nocam:
        outformat = '{:s}/{:s}{:06d}a.rgb'.format(config.dir, config.prefix, num)
        writeto = open(outformat, 'wb')
        try:
#            time.sleep(2) # pause2
            camera.start_preview() 
#            time.sleep(2) # pause2
            #for filename in camera.capture_continuous(outformat, format='rgb'):
            for filename in camera.capture_continuous(writeto, format='rgb'):
                logger.debug("Captured {}".format(filename))
                writeto.close()
#                logger.debug("1")
#                port.write(b'n')
#                logger.debug("2")

#                if b'{OIT:' == portWaitFor2(port, b'FRAMESTOP', b'{OIT:'):
#                    camera.stop_preview()
#                    logger.error("Frame advance timed out [{}], exiting".format(text))
#                    return 1
#                logger.debug("3")
#                num += 1
#                logger.debug("4")
#                outformat = "{:s}/{:s}{:06d}a.rgb".format(config.dir, config.prefix, num)
#                logger.debug("5")
#                writeto = open(outformat, 'w')
#                logger.debug("6")

        except:
            pass
        camera.stop_preview()
        return 0
        
#    for ss,tag in ii:
#        logger.debug("Frame {}/{} shutter {} tag {} tension {}".format(num, numframes, ss, tag, lastTension))
#        if config.raspid:
#            os.kill(config.raspid, signal.SIGUSR1)
#        else:
#            if config.picamera:
#                camera.start_preview()
#                camera.capture("{:s}/{:s}{:05d}a.png".format(config.dir, config.prefix, num))
#                camera.stop_preview()
#            else:
#                captureProc.send_signal(signal.SIGUSR1)
#                (which, text) = captureWaitFor2(captureProc, 
#                    b'Waiting for SIGUSR1 to initiate capture and continue or SIGUSR2 to capture and exit',
#                    b'Failed to run camera app')
#
#                logger.debug("Which {} text {}".format(which, text))
#                if which == 1:
#                    logger.error("captureProc did not initialize [{}], exiting".format(text))
#                    sys.exit(0)


        #args1 = ''.join([BIN, " --header --i2c 0 --expus {0}".format(ss),
        #    Geometry['geo0'], " --fps 1 -t 1000 -sr 1 -o ",
        #    "{:s}/{:s}{:06d}{:s}.raw".format(config.dir, config.prefix, num, tag)])
#        args1 = ''.join([BIN, " -o {} -e png -q 100".format("{:s}/{:s}{:06d}{:s}.png".format(config.dir, config.prefix, num, tag))])
#        runargs = (args1)
#        logger.debug(''.join(runargs))
#        retcode = subprocess.call(''.join(runargs), shell=True, stderr=None)
#        logger.debug("retcode %u" % retcode)
        open("{:s}/{:06d}{:s}.done".format(config.dir, num, tag), "w")
        
    return 0

def exptestframe(port, num):
#    if False == config.nofilm:
#        port.write(b'n')
#        if b'{OIT:' == portWaitFor2(port, b'FRAMESTOP', b'{OIT:'):
#            return 1
    ss = num
    tag = 'a'
    logger.debug("Frame {0} shutter {1} tag {2}".format(num, ss, tag))

    args1 = ''.join([BIN, " --header --i2c 0 --expus {0}".format(ss),
        Geometry['geo0'], " --fps 1 -t 1000 -sr 1 -o ",
        "{:s}/{:s}{:06d}{:s}.raw".format(config.dir, config.prefix, num, tag)])
    runargs = (args1)
    logger.debug(''.join(runargs))
    retcode = subprocess.call(''.join(runargs), shell=True, stderr=None)
    logger.debug("retcode %u" % retcode)
    open("{:s}/{:06d}{:s}.done".format(config.dir, num, tag), "w")

    return 0

def main():
    global intervals
    intervals = [int(mm) for mm in config.intervals.split(',')]
    os.makedirs(config.dir, exist_ok=True);
    files = sorted(glob("{0}/{1}??????a.rgb".format(config.dir, config.prefix)))
    if len(files):
        frameNum = int(files[-1][-11:-5]) + 1
    else:
        frameNum = 0
    logger.debug("Starting at frame {0}".format(frameNum))
    global port
    port = init(frameNum, intervals[0])
    frameCount = 0
    #for frameCount in range(0, config.frames):
    for frameCount in range(0, min(config.frames, numframes)):
        while True:
            statvfs = os.statvfs(config.dev)
            if 20e6 < statvfs.f_frsize * statvfs.f_bavail:
                break;
            else:
                logger.info('Waiting for disk space')
                time.sleep(10)
        if frame(port, frameNum, intervals[frameNum % len(intervals)]): break
        #if exptestframe(port, frameNum): break
        frameNum += 1
    stop(port)
    open("{:s}/{:s}done.done".format(config.dir, config.prefix), "w")

main()

