#!/usr/bin/python

import time
import sys
import pdb
import logging
from logging.handlers import RotatingFileHandler
import os
import httplib
import argparse
import serial
import BeautifulSoup
import telnetlib

SerialPort = '/dev/tty.usbserial-A601KW2O'
CameraIP = ''
CameraPort = 1000

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
logger = logging.getLogger('pcontrol')
fileHandler = logging.FileHandler(filename = './pcontrol.log')
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(lineno)s %(message)s'))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--simulate', action='store_true', help='simulate (no I/O)')
parser.add_argument('--cycles', dest='cycles', help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', help='film type (8mm/super8)')

args = parser.parse_args()

def cameraOn():
    serial.write('c')
    pass

def triple():
    pass

def click(http):
    def subclick(http, cmds):
        for cmd in cmds:
            url = '/api/v1/input/inject?key' % cmd
            http.request('GET', url)
            response = http.getresponse()
            data = response.read()

    subclick(http, ['down=Super_L','down=Super_R'])
    time.sleep(1)
    subclick(http, ['up=Super_L','up=Super_R'])

def armLamp():
    pass

def transferPicture(dir, filename, telnet, http):
#    cameraConn = httplib.HTTPConnection(CameraIP, 80)
    fileurl = '/DCIM/%s/%s' % (dir, filename)
    fileurl = fileurl.encode('ascii', 'ignore')
    http.request('GET', fileurl)
    response = http.getresponse()
    logger.debug('Status %s' % response.status)
    if 200 != response.status:
        logger.error("Status %u getting %s" % (response.status, fileurl))
        return
    data = response.read()
    #cameraConn.close()
    open('%s' % filename, 'w').write(data)
    logger.debug("Downloaded %s, %u bytes" % (fileurl, len(data)))
    logger.debug("Deleting %s/%s" % (TelnetRoot, fileurl))
    telnet.write("rm %s/%s\n" % (TelnetRoot, fileurl))
    telnet.read_very_lazy()
    logger.debug('transferPicture end')


def transferPictures(http):
    numAvailable = 0
    while numAvailable < 3:
        telnet = telnetlib.Telnet(CameraIP)
        telnet.read_until('nx300:/#')
        # wait for at least 3 available images
        telnet.write("find %s/DCIM -name '*.JPG' | wc -l\n" % TelnetRoot)
        tData = telnet.read_until('nx300:/#').replace('\r\n', '\n')
        numAvailable = int(tData.split('\n')[-2])
        logger.debug('numAvailable %u' % numAvailable)
        time.sleep(1)

#     conn = httplib.HTTPConnection(CameraIP, 80)
#     headers = {
#     'Host': CameraIP,
#     'Connection': 'keep-alive',
#     'Upgrade-Insecure-Requests': 1,
#     'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.95 Safari/537.36',
#     'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
#     'Referer': 'http://%s/' % CameraIP,
#     'Accept-Encoding': 'gzip, deflate, sdch',
#     'Accept-Language': 'en-US,en;q=0.8'
#     }
    
    http.request('GET', '/DCIM/') #, None, headers)
    response = http.getresponse()
    #logger.debug('Status %s' % response.status)
    #logger.debug('Reason %s' % response.reason)
    #logger.debug('Headers %s' % response.getheaders())
    data = response.read()
    #logger.debug('Data %s' % data)
    bs=BeautifulSoup.BeautifulSoup(data)
    dirtags=[ee.string for ee in bs.findAll('a') if 'PHOTO' in ee.string]
    for dir in dirtags:
#        conn = httplib.HTTPConnection(CameraIP, 80)
        http.request('GET', '/DCIM/%s' % dir)
        response = http.getresponse()
        fileInfo = response.read()
        bs=BeautifulSoup.BeautifulSoup(fileInfo)
        fileTags=[ee.string for ee in bs.findAll('a') if '.JPG' in ee.string]
#        conn.close()
        for file in fileTags:
            transferPicture(dir, file, telnet, http)
#    conn.close()

def doCycles(serial, http):
    serial.write('2m-100a-')
    click(http)
    serial.write('10m-100a-')
    click(http)
    serial.write('30m-100a-')
    click(http)
    time.sleep(2)
    transferPictures(http)

def main():
    pdb.set_trace()
    logger.debug("Init")
    serPort = serial.Serial(SerialPort, 57600)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    serPort.write(b'cvd') # on, verbose, 8mm

    http = httplib.HTTPConnection(CameraIP, 80, timeout = 5)
    if False == args.simulate:
        time.sleep(2)
        if False == serPort.isOpen():
            logger.error("Cannot open %s, exiting" % SerialPort)
            return 1
        doCycles(serPort, http)

    logger.debug("Closing %s" % SerialPort)
    serPort.write(b'C')
    serPort.close()
    return 0

sys.exit(main())
