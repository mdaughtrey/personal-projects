#!/usr/bin/python3

# Sensor modes
# https://www.raspberrypi.org/documentation/raspbian/applications/camera.md

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
SerialPort="/dev/ttyUSB0"
OUTPUTDIR="/mnt/exthd"
#OUTPUTDIR="/home/mattd/capture"
port = 0
tension = list()
lastTension = 0
numframes = 0
captureProc = None

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
parser.add_argument('--frames', dest='frames', type=int, default=-1, help='frames to capture')
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
config = parser.parse_args()

if config.picamera or config.picameracont:
    camera = PiCamera()
    camera.resolution = (4056, 3040)
    camera.shutter_speed = 700 # 700us


def signal_handler(signal, frame):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    port.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)


def portWaitFor(port, text):
    accum = b''
    logger.debug("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
        #logger.debug("Accumulating %s" % accum)
    logger.debug("Received %s" % text)
    return accum

def portWaitFor2(port, text1, text2):
    accum = b''
    logger.debug("Waiting on %s or %s" % (text1, text2))
    while not text1 in accum and not text2 in accum:
        accum += port.read()
        #logger.debug("Accumulating %s" % accum)
    if text1 in accum:
        logger.debug("Matched on %s" % text1)
        return text1
    if text2 in accum:
        logger.debug("Matched on %s" % text2)
        return text2

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

def init(framenum):
    global tension
    t = Tension()
    global numframes
    (filmlength, numframes, tension) = t.feedstats(config.startdia, config.enddia)
    numframes += 1000
    logger.debug("Length {}m, {} Frames".format(math.floor(filmlength/1000), numframes))

    global lastTension
    lastTension = tension[0]

    # start raspistill
    if 0 == config.raspid:
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

    serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
#    serPort.write(b' ')
#    portWaitFor(serPort, b'{State:Ready}')
    if False == config.noled:
        serPort.write(b'l')
#    serPort.write(b'c%st' % {'8MM': 'd', 'SUPER8': 'D'}[FILMTYPE]) 
    serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film], tension[0]).encode('utf-8'))
    #serPort.write("vc{}{}T".format({'8mm': 'd', 'super8': 'D'}[config.film].encode('utf-8'), tension[0]))
#    portWaitFor(serPort, b'{pt:')
    return serPort

def stop(port):
    port.write(b' ')
    portWaitFor(port, b'{State:Ready}')
    logger.debug("Closing")
    port.close()

def frame(port, num):
    global lastTension
    try:
        if (tension[num] != lastTension) & (num < (len(tension) - 1)):
            lastTension = tension[num]
            port.write('-{}T'.format(tension[num]).encode('utf-8'))
            logger.debug("Set tension {}".format(lastTension))
    except:
        pass

    if False == config.nofilm:
        port.write(b'n')
        if b'{OIT:' == portWaitFor2(port, b'FRAMESTOP', b'{OIT:'):
            logger.error("Frame init timed out [{}], exiting".format(text))
            sys.exit(0)

#    if config.singleframe:
#        ii = zip([SHUTTER[1]], ['a'])
#    else:
#        ii = zip(SHUTTER, ['a','b','c'])
#
#    logger.debug("40")

    if config.picameracont:
        outformat = '{:s}/{:s}{:06d}a.rgb'.format(config.dir, config.prefix, num)
        writeto = open(outformat, 'wb')
        try:
            camera.start_preview()
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
    os.makedirs(config.dir, exist_ok=True);
    files = sorted(glob("{0}/{1}??????a.rgb".format(config.dir, config.prefix)))
#    pdb.set_trace()
    if len(files):
        frameNum = int(files[-1][-11:-5]) + 1
    else:
        frameNum = 0
    logger.debug("Starting at frame {0}".format(frameNum))
    global port
    port = init(frameNum)
    frameCount = 0
    #for frameCount in range(0, config.frames):
    for frameCount in range(0, numframes):
        while True:
            statvfs = os.statvfs(config.dev)
            if 20e6 < statvfs.f_frsize * statvfs.f_bavail:
                break;
            else:
                logger.info('Waiting for disk space')
                time.sleep(10)
        if frame(port, frameNum): break
        #if exptestframe(port, frameNum): break
        frameNum += 1
    stop(port)
    open("{:s}/{:s}done.done".format(config.dir, config.prefix), "w")

main()

