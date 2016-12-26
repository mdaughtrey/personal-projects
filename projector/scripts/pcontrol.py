#!/usr/bin/python

import time
import io
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
import socket

SerialPort = '/dev/tty.usbserial-A601KW2O'
CameraIP = '192.168.0.33'
CameraPort = 1000
TelnetRoot = '/mnt/mmc'

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
logger = logging.getLogger('pcontrol')
fileHandler = logging.FileHandler(filename = './pcontrol.log')
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(lineno)s %(message)s'))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--simulate', action='store_true', help='simulate (no I/O)')
parser.add_argument('--cycles', dest='cycles', default=10, type=int, help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', help='film type (8mm/super8)')

args = parser.parse_args()

def cameraOn():
    serial.write('c')
    pass

def triple():
    pass

def click(cb):
    def subclick(cmds):
        for cmd in cmds:
#            pdb.set_trace()
            url = '/api/v1/input/inject?key%s' % cmd
            while True:
                try:
                    http = httplib.HTTPConnection(CameraIP, 80, timeout = 5)
                    http.request('GET', url)
                    break
                except socket.error as ee:
                    logger.debug("No connection, retry in 1 sec")
                    time.sleep(5)

            response = http.getresponse()
            data = response.read()

    cb()
    subclick(['down=Super_L','down=Super_R'])
    #time.sleep(1)
    subclick(['up=Super_L','up=Super_R'])

def armLamp():
    pass

def transferPicture(dir, filename, telnet):
    http = httplib.HTTPConnection(CameraIP, 80)
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


def transferPictures(telnet):
    logger.debug('transferPictures')
    numAvailable = 0
    while numAvailable < 1:
        # wait for at least 3 available images
        telnet.write("find %s/DCIM -name '*.JPG' | wc -l\n" % TelnetRoot)
#        pdb.set_trace()
        tData = ''
        while True:
            rx = telnet.read_very_eager()
            if '' == rx:
                break
            time.sleep(0.5)
            tData += rx
        logger.debug(tData)

        for elem in tData.split('\r\n'):
            try:
                numAvailable = int(elem)
            except:
                pass
        logger.debug('numAvailable %u' % numAvailable)
        time.sleep(0.1)

    http = httplib.HTTPConnection(CameraIP, 80)
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
        http = httplib.HTTPConnection(CameraIP, 80)
        http.request('GET', '/DCIM/%s' % dir)
        response = http.getresponse()
        fileInfo = response.read()
        bs=BeautifulSoup.BeautifulSoup(fileInfo)
        fileTags=[ee.string for ee in bs.findAll('a') if '.JPG' in ee.string]
#        conn.close()
        for file in fileTags:
            transferPicture(dir, file, telnet)
#    conn.close()

def doCycles(serial, telnet):
    def waitCardOps(telnet):
        logger.debug("Waiting on card ops")
        telnet.write('/mnt/mmc/moncard.sh\n')
        rx = telnet.read_until("Timeout")
        logger.debug("moncard.sh returns %s" % rx)
        if "Wait " in rx:
            logger.error("Error waiting on SD card activity")

    for ii in xrange(1, args.cycles):
        logger.debug("Frame %u of %u" % (ii, args.cycles))
        logger.debug("Click 1")
        click(lambda: serial.write('2m-200a-'))
#        waitFor(serial, '{LampDone}')
        waitCardOps(telnet)
        transferPictures(telnet)
        logger.debug("Click 2")
        click(lambda: serial.write('10m-200a-'))
#//        waitFor(serial, '{LampDone}')
        waitCardOps(telnet)
        transferPictures(telnet)
        logger.debug("Click 3")
        click(lambda: serial.write('30m-200a-'))
#//        waitFor(serial, '{LampDone}')
        waitCardOps(telnet)
        transferPictures(telnet)

def waitFor(port, text):
    accum = ''
    logger.debug("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
#        logger.debug('accum %s' % accum)
    logger.debug("Received %s" % text)
    return accum

def main():
    logger.debug("Init")
    serPort = serial.Serial(SerialPort, 57600)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    waitFor(serPort, '{State:Ready}')
    serPort.write(b'cd') # on, verbose, 8mm
    waitFor(serPort, '{mode:8mm}')

    if False == args.simulate:
        time.sleep(2)
        if False == serPort.isOpen():
            logger.error("Cannot open %s, exiting" % SerialPort)
            return 1
        telnet = telnetlib.Telnet(CameraIP)
        telnet.read_until('nx300:/#')
        doCycles(serPort, telnet)

    logger.debug("Closing %s" % SerialPort)
    serPort.write(b'C')
    serPort.close()
    return 0

sys.exit(main())
