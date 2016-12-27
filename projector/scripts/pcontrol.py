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
import requests
import urllib

SerialPort = '/dev/tty.usbserial-A601KW2O'
CameraIP = '192.168.0.33'
CameraPort = 1000
TelnetRoot = '/mnt/mmc'
ControllerIP = '192.168.0.18'
ControllerPort = 5000

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
logger = logging.getLogger('pcontrol')
fileHandler = logging.FileHandler(filename = './pcontrol.log')
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(lineno)s %(message)s'))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--simulate', action='store_true', help='simulate (no I/O)')
parser.add_argument('--cycles', dest='cycles', default=10, type=int, help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', required=True, choices=['8mm','s8'], help='film type')
parser.add_argument('--mode', dest='mode', required=True, choices=['upload','pipeline'], help='upload a file')
parser.add_argument('--project', dest='project', required=True, help='set project name')
parser.add_argument('--container', dest='container', required=True, help='set container name')
parser.add_argument('--filename', dest='filename', help='set filename')
args = parser.parse_args()

def click(cb):
    def subclick(cmds):
        for cmd in cmds:
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
    subclick(['up=Super_L','up=Super_R'])

def transferPicture(dir, filename, telnet):
    http = httplib.HTTPConnection(CameraIP, 80)
    fileurl = '/DCIM/%s/%s' % (dir, filename)
    fileurl = fileurl.encode('ascii', 'ignore')
    logger.debug("pre GET")
    http.request('GET', fileurl)
    logger.debug("post GET")
    response = http.getresponse()
    logger.debug('Status %s' % response.status)
    if 200 != response.status:
        logger.error("Status %u getting %s" % (response.status, fileurl))
        return
    logger.debug("pre response.read")
    data = response.read()
    logger.debug("post response.read")
    #    open('%s' % filename, 'w').write(data)
    logger.debug("Downloaded %s, %u bytes" % (fileurl, len(data)))

    pdb.set_trace()
    hargs = (ControllerIP, ControllerPort, args.project, dir, filename)
    hUrl = 'http://%s:%u/upload?project=%s&container=%s&file=%s' % hargs
    response = requests.put(url=hUrl, data=data,
        headers={'Content-Type': 'application/octet-stream'})

    logger.debug("Uploaded %s, %u bytes" % (fileurl, len(data)))
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
            transferPicture(dir.replace('/',''), file, telnet)
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
        serial.write('n')
        transferPictures(telnet)

def waitFor(port, text):
    accum = ''
    logger.debug("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
#        logger.debug('accum %s' % accum)
    logger.debug("Received %s" % text)
    return accum

#def setProject(name):
#    http = httplib.HTTPConnection(ControllerIP, ControllerPort)
#    http.request('GET', '/command?project=%s' % name)
#    response = http.getresponse()
#    data = response.read()
#    logger.debug(data)

def uploadFile(project, container, filename):
    if False in (project, container, filename):
        logger.error("Undefined parameter")
        return 1

    hargs = (ControllerIP, ControllerPort, project, container, filename)
    hUrl = 'http://%s:%u/upload?project=%s&container=%s&file=%s' % hargs
    response = requests.put(url=hUrl, data=open(filename).read(),
        headers={'Content-Type': 'application/octet-stream'})
    return 0

#    http = httplib.HTTPConnection(ControllerIP, ControllerPort)
#    hargs = (project, container, filename)
#    url = '/upload?project=%s&container=%s&file=%s' % hargs
#    http.putrequest('PUT', url)
#    #http.putheader('Content-Length', str(os.stat(filename).st_size))
#    http.putheader('Content-Length', '10')
#    http.endheaders()
#    http.send("0123456789")
#    #http.send(open(filename).read())
#    response = http.getresponse()
#    pdb.set_trace()
#    data = response.read()
#    logger.debug(data)


#def procTriple(project, container, filenames):
#    if False in (project, container, filenames):
#        logger.error("Undefined parameter")
#        return 1
#
#    hargs = (ControllerIP, ControllerPort, project, container, urllib.parse.quote(filenames))
#    hUrl = 'http://%s:%u/triple?project=%s&container=%s&files=%s' % hargs
#    response = requests.get(hUrl)
#    return 0

def main():
    logger.debug("Init")
    if 'upload' == args.mode:
        return uploadFile(args.project, args.container, args.filename)
    if 'pipeline' != args.mode:
        return 1

    serPort = serial.Serial(SerialPort, 57600)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    waitFor(serPort, '{State:Ready}')
    serPort.write(b'c%st' % {'8mm': 'd', 's8': 'D'}[args.filmtype]) # on, verbose, 8mm
#    waitFor(serPort, '{mode:8mm}')

    if False == args.simulate:
        time.sleep(2)
        if False == serPort.isOpen():
            logger.error("Cannot open %s, exiting" % SerialPort)
            return 1
        telnet = telnetlib.Telnet(CameraIP)
        telnet.read_until('nx300:/#')
        doCycles(serPort, telnet)

    logger.debug("Closing %s" % SerialPort)
    serPort.write(b'TC')
    serPort.close()
    return 0

sys.exit(main())
