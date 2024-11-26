#!/usr/bin/python

import requests
import os
import re
import time
import pdb
import socket
import logging
from logging.handlers import RotatingFileHandler
import code
import signal
import glob
import argparse
import sys

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
FormatString='%(asctime)s %(name)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('fup')
fileHandler = logging.FileHandler(filename = './fup.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

ss = requests.Session()
parser = argparse.ArgumentParser()
parser.add_argument('--dir', dest='dir', help='image dir')
parser.add_argument('--ip', dest='ip', help='controller ip')
parser.add_argument('--port', dest='port', type=int, help='controller port')
#parser.add_argument('--justupload', dest='justupload', action='store_true', default='false')
parser.add_argument('--nodelete', dest='delete', action='store_false', default='True')
parser.add_argument('--frames', dest='frames', type=int)
args = parser.parse_args()

donedone = args.dir + "/done.done"
signalled = False
def signal_handler(signal, frame):
    logger.debug("Control C, touch done file")
    os.utime(donedone, None)
    signalled = True

def getFiles():
    for file in sorted(glob.glob("%s/reference*.done" % args.dir)):
        if os.path.isfile(file.replace('done', 'raw')):
            yield (file.replace("done", "raw"), file)

    for file in sorted(glob.glob("%s/???????.done" % args.dir)):
        if os.path.isfile(file.replace('done', 'raw')):
            yield (file.replace("done", "raw"), file)

#    for file in sorted(glob.glob("%s/??????.done" % args.dir)):
#        if os.path.isfile(file.replace('done', 'jpg')):
#            yield (file.replace("done", "jpg"), file)

def markDone(filename):
    os.remove(filename.replace("jpg","done"))

def uploader():
    logger.debug("Uploader Starts")
    uploaded = 0
    frameCount = 0
    for image, done in getFiles():
    #for image, done in zip(['000000a.raw','000000b.raw','000000c.raw'],
    #    ['000000a.done','000000b.done','000000c.done']):
    #    image = '/home/mattd/capture/%s' % image
    #    done = '/home/mattd/capture/%s' % done
        #if signalled:
        #    return 0
        logger.debug("uploader gets %s" % image)
#        hargs = (args.ip, args.port)
        splits = image.split('/')
        reference = False
        if splits[-1][:9] == 'reference':
            reference = True
            hUrl = 'http://%s:%u/reference' % (args.ip, args.port)
        else:
            hUrl = 'http://%s:%u/upload/%s' % (args.ip, args.port, splits[-1][6])

        try:
            logger.debug(hUrl)
            if reference:
                response = ss.put(url=hUrl, data=open(image).read(),
                    params = {'refindex': re.search('reference(\d+).raw',splits[4]).groups()[0]},
                    headers={'Content-Type': 'application/octet-stream'})
            else:
                response = ss.put(url=hUrl, data=open(image).read(),
                    headers={'Content-Type': 'application/octet-stream'})
                if False == response.ok:
                    logger.error("Upload failed %u: %s" % (response.status_code, response.reason))
                    return -1

            logger.debug("Uploaded %s" % hUrl)
            if args.delete:
                logger.debug("removing %s" % image)
                os.remove(done)
                os.remove(image)
            frameCount += 1
            logger.debug("FrameCount %u" % frameCount)
            uploaded += 1
            if args.frames and frameCount > args.frames:
               logger.debug("Uploaded %u frames" % frameCount)
               pdb.set_trace()
               open("%s/done.done" % args.dir, "w")
               return 0

        except Exception as ee:
            logger.error("HTTP upload fail %s" % str(ee))
            logger.debug("Uploaded %u files" % uploaded)
            return -1
    return uploaded

#if os.path.isfile(donedone):
#    os.remove(donedone)

while True:
    rc = uploader()
    if rc < 0:
        sys.exit(1)
    if 0 == rc and os.path.isfile(donedone):
       hargs = (args.ip, args.port)
       hUrl = 'http://%s:%u/upload' % hargs
       try:
           logger.debug(hUrl)
           response = ss.get(url=hUrl) 

       except Exception as ee:
           logger.error("HTTP Upload Done fails %s" % str(ee))
       if False == args.delete:
           os.remove(donedone)
       sys.exit(0)

    else:
        logger.debug("Sleeping");
        time.sleep(2)

