#!/usr/bin/python

import requests
import os
import sqlite3
import time
import pdb
import socket
import logging
from logging.handlers import RotatingFileHandler
import code
import signal

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
FormatString='%(asctime)s %(name)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('uploader')
fileHandler = logging.FileHandler(filename = './uploader.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

dbName="/var/tmp/pcontroldb"
ControllerIP = '192.168.0.18'
ControllerPort = 5000

ss = requests.Session()

def initDb():
    if os.path.isfile(dbName):
        os.remove(dbName)
    logger.debug("Creating %s" % dbName)
    conn = sqlite3.connect(dbName)
    cur = conn.cursor()
    cur.execute('CREATE TABLE pending (filename TEXT)')
    conn.commit()
    conn.close()

def getNextUploadFile():
    try:
       conn = sqlite3.connect(dbName)
       cur = conn.cursor()
       cur.execute("SELECT filename FROM pending ORDER BY rowid LIMIT 1")
       data = cur.fetchall()
       if 0 == len(data):
           return None
       return data[0][0].encode('ascii', 'ignore')

    except Exception as ee:
        logger.error("getNextUpload file failed %s" % str(ee))

def markUploaded(filename):
    try:
        conn = sqlite3.connect(dbName)
        cur = conn.cursor()
        cur.execute("DELETE FROM pending WHERE filename='%s'" % filename)
        conn.commit()
    except Exception as ee:
        logger.error("markUploaded file failed %s" % str(ee))

def uploader():
    logger.debug("Uploader Starts")
    while True:
        filename = getNextUploadFile()
        if filename is None:
            time.sleep(2)
            continue
        logger.debug("uploader gets %s" % filename)
        if 'exit' == filename:
            markUploaded(filename)
            logger.debug("uploader exits")
            return

        hargs = (ControllerIP, ControllerPort)
        hUrl = 'http://%s:%u/upload' % hargs
        try:
            logger.debug(hUrl)
            response = ss.put(url=hUrl, data=open(filename).read(),
                headers={'Content-Type': 'application/octet-stream'})
            logger.debug("Uploaded %s" % hUrl)
            markUploaded(filename)
            logger.debug("removing %s" % filename)
            os.remove(filename)
        except Exception as ee:
            logger.error("HTTP upload fail %s" % str(ee))

#initDb()
uploader()
