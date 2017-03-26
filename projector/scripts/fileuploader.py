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

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
FormatString='%(asctime)s %(name)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('fup')
fileHandler = logging.FileHandler(filename = './fup.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

ControllerIP = '192.168.0.18'
ControllerPort = 5000

ss = requests.Session()

parser = argparse.ArgumentParser()
parser.add_argument('--dir', dest='dir', help='image dir')
args = parser.parse_args()



def getFiles():
    for file in sorted(glob.glob("%s/*.done" % args.dir)):
        yield (file.replace("done", "jpg"), file)

def markDone(filename):
    os.remove(filename.replace("jpg","done"))

def uploader():
    logger.debug("Uploader Starts")
    for jpg, done in getFiles():
       logger.debug("uploader gets %s" % jpg)
       hargs = (ControllerIP, ControllerPort)
       hUrl = 'http://%s:%u/upload' % hargs
       try:
           logger.debug(hUrl)
           response = ss.put(url=hUrl, data=open(jpg).read(),
               headers={'Content-Type': 'application/octet-stream'})
           logger.debug("Uploaded %s" % hUrl)
           logger.debug("removing %s" % jpg)
           os.remove(done)
           os.remove(jpg)

       except Exception as ee:
           logger.error("HTTP upload fail %s" % str(ee))

while True:
    uploader()
    logger.debug("Sleeping");
    time.sleep(60)
