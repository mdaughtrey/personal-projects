#!/usr/bin/python

import requests
import os
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
args = parser.parse_args()

donedone = args.dir + "/done.done"

def getFiles():
    for file in sorted(glob.glob("%s/reference?.done" % args.dir)):
        if os.path.isfile(file.replace('done', 'raw.bz2')):
            yield (file.replace("done", "raw.bz2"), file)

    for file in sorted(glob.glob("%s/??????.done" % args.dir)):
        if os.path.isfile(file.replace('done', 'jpg')):
            yield (file.replace("done", "jpg"), file)

def markDone(filename):
    os.remove(filename.replace("jpg","done"))

def uploader():
    logger.debug("Uploader Starts")
    uploaded = 0
    for image, done in getFiles():
        logger.debug("uploader gets %s" % image)
        hargs = (args.ip, args.port)
        splits = image.split('/')
        reference = False
        if splits[2][:9] == 'reference':
            reference = True
            hUrl = 'http://%s:%u/reference' % hargs
        else:
            hUrl = 'http://%s:%u/upload' % hargs

        try:
            logger.debug(hUrl)
            if reference:
                response = ss.put(url=hUrl, data=open(image).read(),
                    params = {'refindex': splits[2][9]},
                    headers={'Content-Type': 'application/octet-stream'})
            else:
                response = ss.put(url=hUrl, data=open(image).read(),
                    headers={'Content-Type': 'application/octet-stream'})

            logger.debug("Uploaded %s" % hUrl)
            logger.debug("removing %s" % image)
#            os.remove(done)
#            os.remove(image)
            uploaded += 1

        except Exception as ee:
            logger.error("HTTP upload fail %s" % str(ee))
            logger.debug("Uploaded %u files" % uploaded)
    return uploaded

if os.path.isfile(donedone):
    os.remove(donedone)

while True:
    if 0 == uploader() and os.path.isfile(donedone):
       hargs = (args.ip, args.port)
       hUrl = 'http://%s:%u/upload' % hargs
       try:
           logger.debug(hUrl)
           response = ss.get(url=hUrl) 

       except Exception as ee:
           logger.error("HTTP Upload Done fails %s" % str(ee))
       os.remove(donedone)
       sys.exit(0)

    else:
        logger.debug("Sleeping");
        time.sleep(2)

