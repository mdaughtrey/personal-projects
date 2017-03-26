#!/usr/bin/python

import time
import io
import sys
import pdb
import logging
from logging.handlers import RotatingFileHandler
import os
<<<<<<< HEAD
import httplib
import argparse
import serial
#import BeautifulSoup
import telnetlib
import socket
import requests
import urllib
import stat
from multiprocessing import Process
import tempfile
import re
import glob
import threading
import sqlite3

#Fifoname = "/var/tmp/pcontrol.fifo"
dbName="/var/tmp/pcontroldb"
dbLock = threading.Lock()
SerialPort = '/dev/tty.usbserial-A601KW2O'
CameraIP = '0.0.0.0'
CameraPort = 1000
=======
import argparse
import serial
import telnetlib
import requests
import tempfile
import re
import sqlite3
import code
import signal

ctlSession = requests.Session()
camSession = requests.Session()
CtlTimeout = 1
CamTimeout = 2

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
#Fifoname = "/var/tmp/pcontrol.fifo"
dbName="/var/tmp/pcontroldb"
#dbLock = threading.Lock()
SerialPort = '/dev/tty.usbserial-A601KW2O'
CameraIP = '0.0.0.0'
CameraPort = 80
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
TelnetRoot = '/mnt/mmc'
ControllerIP = '192.168.0.18'
ControllerPort = 5000

<<<<<<< HEAD
FormatString='%(asctime)s %(process)d %(levelname)s %(lineno)s %(message)s'
=======
FormatString='%(asctime)s %(name)s %(levelname)s %(lineno)s %(message)s'
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('pcontrol')
fileHandler = logging.FileHandler(filename = './pcontrol.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
<<<<<<< HEAD
parser.add_argument('--simulate', action='store_true', help='simulate (no I/O)')
parser.add_argument('--cycles', dest='cycles', default=10, type=int, help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', required=True, choices=['8mm','s8'], help='film type')
parser.add_argument('--mode', dest='mode', required=True, choices=['gentitle','genmovie','fifotest','upload','pipeline'], help='upload a file')
=======
parser.add_argument('--cycles', dest='cycles', default=10, type=int, help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', required=True, choices=['8mm','s8'], help='film type')
parser.add_argument('--mode', dest='mode', required=True, choices=['gentitle','genmovie','fifotest','upload','pipeline','uploader'])
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
parser.add_argument('--filename', dest='filename', help='set filename')
parser.add_argument('--noupload', dest='noupload', action='store_true', help='disable upload')
parser.add_argument('--inline', dest='inline', action='store_true')
args = parser.parse_args()
upl = None

def initDb():
    if os.path.isfile(dbName):
        os.remove(dbName)
    logger.debug("Creating %s" % dbName)
    conn = sqlite3.connect(dbName)
    cur = conn.cursor()
    cur.execute('CREATE TABLE pending (filename TEXT)')
    conn.commit()
    conn.close()

def putUploadFile(filename):
<<<<<<< HEAD
    with dbLock:
=======
    try:
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        conn = sqlite3.connect(dbName)
        cur = conn.cursor()
        cur.execute("INSERT INTO pending (filename) values('%s')" % filename)
        conn.commit()
        conn.close()

<<<<<<< HEAD
def getNextUploadFile():
    with dbLock:
        conn = sqlite3.connect(dbName)
        cur = conn.cursor()
        cur.execute("SELECT filename FROM pending ORDER BY rowid LIMIT 1")
        data = cur.fetchall()
        if 0 == len(data):
            return None
        return data[0][0].encode('ascii', 'ignore')

def markUploaded(filename):
    with dbLock:
        logger.debug("conn")
        conn = sqlite3.connect(dbName)
        logger.debug("cur")
        cur = conn.cursor()
        logger.debug("exec")
        cur.execute("DELETE FROM pending WHERE filename='%s'" % filename)
        logger.debug("commit")
        conn.commit()
        logger.debug("done")
    
def uploader():
    logger.debug("uploader")
=======
    except Exception as ee:
       logger.error("putUploadFile fails %s" % str(ee))

def getNextUploadFile():
#    with dbLock:
    conn = sqlite3.connect(dbName)
    cur = conn.cursor()
    cur.execute("SELECT filename FROM pending ORDER BY rowid LIMIT 1")
    data = cur.fetchall()
    if 0 == len(data):
        return None
    return data[0][0].encode('ascii', 'ignore')

def markUploaded(filename):
#    with dbLock:
    conn = sqlite3.connect(dbName)
    cur = conn.cursor()
    cur.execute("DELETE FROM pending WHERE filename='%s'" % filename)
    conn.commit()
    
def uploader():
    logger.debug("Uploader Starts")
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
    while True:
        filename = getNextUploadFile()
        if filename is None:
            time.sleep(2)
            continue
        logger.debug("uploader gets %s" % filename)
        if 'exit' == filename:
            logger.debug("uploader exits")
            return

        hargs = (ControllerIP, ControllerPort)
        hUrl = 'http://%s:%u/upload' % hargs
        try:
            logger.debug(hUrl)
<<<<<<< HEAD
            response = requests.put(url=hUrl, data=open(filename).read(),
=======
            response = ctlSession.put(url=hUrl, data=open(filename).read(),
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
                headers={'Content-Type': 'application/octet-stream'})
            logger.debug("Uploaded %s" % hUrl)
        except:
            logger.error("HTTP upload fail") # , %s" % ee.message)

        os.remove(filename)
        markUploaded(filename)

#def uploadToController(filename):
#    hargs = (ControllerIP, ControllerPort, args.project)
#    hUrl = 'http://%s:%u/upload?project=%s' % hargs
#    try:
#        logger.debug(hUrl)
#        response = requests.put(url=hUrl, data=open(filename).read(),
#            headers={'Content-Type': 'application/octet-stream'})
#        logger.debug("Uploaded %s" % hUrl)
#    except:
#        logger.error("HTTP upload fail") # , %s" % ee.message)

<<<<<<< HEAD
def subclick(cmds):
    for cmd in cmds:
        url = '/api/v1/input/inject?key%s' % cmd
        while True:
            try:
                logger.debug('Contact %s' % CameraIP)
                http = httplib.HTTPConnection(CameraIP, 80, timeout = 5)
                http.request('GET', url)
                break
            except socket.error as ee:
                logger.debug("subclick No connection, retry in 1 sec")
                time.sleep(5)

        response = http.getresponse()
        data = response.read()

def transferPicture(dir, filename, telnet):
    http = httplib.HTTPConnection(CameraIP, 80)
    fileurl = '/DCIM/%s/%s' % (dir, filename)
    fileurl = fileurl.encode('ascii', 'ignore')
    http.request('GET', fileurl)
    response = http.getresponse()
    if 200 != response.status:
        logger.error("Status %u getting %s" % (response.status, fileurl))
        return
    imgdata = response.read()
    logger.debug("Downloaded %s, %u bytes" % (fileurl, len(imgdata)))
=======
def urlRetry(url, callback):
    retries = 3
    while retries:
        try:
            logger.debug(url)
            callback()
            return True

        except Exception as ee:
            logger.warn("%s error, retrying" % url)
            time.sleep(1)
            retries -= 1

    logger.debug("%s timed out, giving up" % url)
    return False

def subclick(cmds):
    for cmd in cmds:
        url = 'http://%s/api/v1/input/inject?key%s' % (CameraIP, cmd)
        #return urlRetry(url, lambda: camSession.get(url, timeout = CamTimeout))

        retries = 3
        while retries:
            try:
                logger.debug('Contact %s' % CameraIP)
                camSession.get(url, timeout=CamTimeout)
                break
            except Exception as ee:
                logger.warn("subclick error %s" % str(ee))
                time.sleep(1)
                retries -= 1

    if 0 == retries:
        logger.debug("Too many retries, giving up")
        return False
    return True

def transferPicture(dir, filename, telnet):
    fileurl = ('/DCIM/%s/%s' % (dir, filename)).encode('ascii', 'ignore')
    url = 'http://%s%s' % (CameraIP, fileurl)
#    if False == urlRetry(url, lambda: camSession.get(url, timeout = CamTimeout)):
#        return False
    retries = 3
    while retries:
        try:
            response = camSession.get(url, timeout=CamTimeout)
            if 200 != response.status_code:
                logger.error("Status %u getting %s" % (response.status, url))
                return
            logger.debug("Downloaded %s, %u bytes" % (fileurl, len(response.content)))
            break
        except Exception as ee:
            logger.error("transferPicture fails, %s" % str(ee))
            time.sleep(1)
            retries -= 1

    if 0 == retries:
        logger.error("Too many retries, giving up")
        return False
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

    if False == args.noupload:
        imgfile = tempfile.NamedTemporaryFile(delete = False, prefix='pcntl')
        imgname = imgfile.name
<<<<<<< HEAD
        imgfile.write(imgdata)
=======
        imgfile.write(response.content)
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        imgfile.close()
        logger.debug("Wrote imagedata to %s" % imgname)
        putUploadFile(imgname)
        if args.inline:
<<<<<<< HEAD
            pdb.set_trace()
=======
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
            uploader()

    logger.debug("Deleting %s/%s" % (TelnetRoot, fileurl))
    telnet.write("rm %s/%s > /dev/null\n" % (TelnetRoot, fileurl))
<<<<<<< HEAD
    tData = telnet.read_until('nx300:/#')
    logger.debug('transferPicture end')
=======
    logger.debug("read_until nx300")
    tData = telnet.read_until('nx300:/#')
    logger.debug('transferPicture end')
    return True
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22


def transferPictures(telnet):
    logger.debug('transferPictures')
    numAvailable = 0
    while numAvailable < 3:
        # wait for at least 3 available images
        telnet.write("ls %s/DCIM/*/*.JPG | tail -3\n" % TelnetRoot)
        tData = telnet.read_until('nx300:/#')
        images = [ee for ee in tData.split('\r\n') if 'PHOTO/SAM' in ee]
        numAvailable = len(images)

    for image in images:
        ee = image.split('/')[-2:]
<<<<<<< HEAD
        transferPicture(ee[0], ee[1], telnet)

def doCycles(serial, telnet):
    logger.debug("Deleting extraneous camera images")
    telnet.write("rm -rf %s/DCIM/* > /dev/null\n" % TelnetRoot)
    telnet.read_until('nx300:/#')
=======
        if False == transferPicture(ee[0], ee[1], telnet):
            return False
    return True

def doCycles(serial, telnet, numCycles):
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
    def waitCardOps(telnet):
        logger.debug("Waiting on card ops")
        telnet.write('/mnt/mmc/moncard.sh\n')
        rx = ''
        while True:
            rx += telnet.read_some()
            logger.debug("rx %s" % rx)
            if "Timeout" in rx:
                break
#        rx = telnet.read_until("Timeout")
        logger.debug("moncard.sh returns %s" % rx)
        if "Wait " in rx:
            logger.error("Error waiting on SD card activity")


<<<<<<< HEAD
    for ii in xrange(1, args.cycles + 1):
=======
    for ii in xrange(1, numCycles + 1):
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        if serialWaitFor(serial, '{OIT:'):
            logger.debug("OIT")
            sys.exit(0)
        logger.debug("Frame %u of %u" % (ii, args.cycles))
        serial.write('200a')
<<<<<<< HEAD
        subclick(['down=Super_L','down=Super_R'])

        while True == serial.getCTS():
            pass

        subclick(['up=Super_L','up=Super_R'])
        serial.write('n')
        transferPictures(telnet)
=======
        if False == subclick(['down=Super_L','down=Super_R']):
            return ii

        now = time.clock() 
        while True == serial.getCTS():
            if (time.clock() - now) > 3.0:
                logger.error("CTS loop timed out")
                return ii

        if False == subclick(['up=Super_L','up=Super_R']):
            return ii
        serial.write('n')
        if False == transferPictures(telnet):
            return ii

    return ii
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

def serialWaitFor(serial, text):
    accum = ''
    while serial.inWaiting():
        accum += serial.read()

    if text in accum:
        return True

    return False

def portWaitFor(port, text):
    accum = ''
    logger.debug("Waiting on %s" % text)
    while not text in accum:
        accum += port.read()
    logger.debug("Received %s" % text)
    return accum

def uploadFile(filename):
    putUploadFile(filename)
    return 0

<<<<<<< HEAD
def uploaderInit():
    initDb()
    if False == args.inline:
        global upl
        upl = Process(target = uploader)
        upl.start()
        logger.debug("Started uploader")
=======
#def uploaderInit():
#    initDb()
#    if False == args.inline:
#        global upl
#        upl = Process(target = uploader)
#        upl.start()
#        logger.debug("Started uploader")
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

def genMovie():
    try:
        hargs = (ControllerIP, ControllerPort)
        hUrl = 'http://%s:%u/genmovie' % hargs
<<<<<<< HEAD
        logger.debug(hUrl)
        response = requests.get(url=hUrl)
    except:
        logger.error("HTTP genmovie fail") # , %s" % ee.message)
=======
#    return urlRetry(url, lambda: ctlSession.get(hUrl, timeout=CtlTimeout))
        logger.debug(hUrl)
        response = ctlSession.get(hUrl, timeout=CtlTimeout)
    except:
        logger.error("HTTP genmovie fail") # , %s" % ee.message)
    return True
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22

def uploadTitleFiles(filenames):
    def uploadFile(filename, mode):
        match = re.match('title(\d+).txt', filename)
        hargs = (ControllerIP, ControllerPort, mode, match.group(1))
        hUrl = 'http://%s:%u/%s?page=%s' % hargs
        try:
            logger.debug(hUrl)
<<<<<<< HEAD
            response = requests.put(url=hUrl, data=open(filename).read(),
=======
            response = ctlSession.put(url=hUrl, data=open(filename).read(),
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
                headers={'Content-Type': 'application/octet-stream'})
            logger.debug("Uploaded %s" % hUrl)
        except:
            logger.error("HTTP upload fail") # , %s" % ee.message)
            sys.exit(1)

    for filename in filenames[:-1]:
        uploadFile(filename, 'titlefile')
    uploadFile(filenames[-1], 'gentitle')

#
# query router for camera IP
#
def getCameraIP():
    uheaders = {
    'Host': '192.168.0.1',
    'Connection': 'keep-alive',
    'Content-Length': '42',
    'Content-Type': 'application/x-www-form-urlencoded',
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
    }
    payload='loginUsername=admin&loginPassword=N0stromo'

    hUrl = 'http://192.168.0.1/goform/login'
    response = requests.post(url=hUrl, headers=uheaders, data = payload)
    ipAddress = ''
    if response.ok:
        while True:
<<<<<<< HEAD
            logger.debug("Waiting for camera")
=======
            logger.debug("Waiting for router")
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
            time.sleep(1)
            response = requests.get('http://192.168.0.1/wlanAccess.asp')
            if not response.ok:
                continue
            htmlClip = re.search('<td>(\d+\.\d+\.\d+\.\d+)</td><td>nx300</td>', response.content)
            if htmlClip:
                ipAddress = htmlClip.groups(0)[0]
                logger.debug("Camera IP is %s" % ipAddress)
                return ipAddress

<<<<<<< HEAD
def main():
    logger.debug("Init")
    if args.mode in ('gentitle'):
=======
def waitForCamera(telnet):
    logger.debug("waitForCamera")
    telnet.read_until('nx300:/#')
    logger.debug("Deleting extraneous camera images")
    telnet.write("rm -rf %s/DCIM/* > /dev/null\n" % TelnetRoot)
    telnet.read_until('nx300:/#')

    while True:
        time.sleep(3)
        if True == subclick(['down=Super_L','down=Super_R']):
            time.sleep(0.5)
            subclick(['up=Super_L','up=Super_R'])
            break

        if True == subclick(['up=Super_L','up=Super_R']):
            break

    logger.debug("Camera ready")

def main():
    logger.debug("Init")
    if args.mode in ('gentitle'):
        import glob
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        uploadTitleFiles(glob.glob(args.filename))
        return 0
    if 'genmovie' == args.mode:
        genMovie()
        return 0
<<<<<<< HEAD
    if False == args.noupload:
        uploaderInit()
=======
#    if False == args.noupload:
#        uploaderInit()
    if 'uploader' == args.mode:
        uploader()
        sys.exit(0)
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
    if 'upload' == args.mode:
        return uploadFile(args.filename)
    if 'pipeline' != args.mode:
        return 1

<<<<<<< HEAD
    serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
    logger.debug("Opening %s" % SerialPort)
    if serPort.isOpen():
        serPort.close()
    serPort.open()
    portWaitFor(serPort, '{State:Ready}')
    # Camera on, film type, autotension
    serPort.write(b'c%st' % {'8mm': 'd', 's8': 'D'}[args.filmtype]) 
#    portWaitFor(serPort, '{mode:8mm}')

    if False == args.simulate:
=======
    def cameraOff(serPort):
        logger.debug("Closing %s" % SerialPort)
        serPort.write(b'TC')
        serPort.close()

    numCycles = args.cycles
    doneCycles = 0
    while True:
        serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
        logger.debug("Opening %s" % SerialPort)
        if serPort.isOpen():
            serPort.close()

        serPort.open()
        portWaitFor(serPort, '{State:Ready}')
        # Camera on, film type, autotension
        serPort.write(b'c%st' % {'8mm': 'd', 's8': 'D'}[args.filmtype]) 
        serPort.write('n')

>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
        time.sleep(2)
        if False == serPort.isOpen():
            logger.error("Cannot open %s, exiting" % SerialPort)
            return 1
<<<<<<< HEAD
        global CameraIP
        CameraIP = getCameraIP()
        telnet = telnetlib.Telnet(CameraIP)
        telnet.read_until('nx300:/#')
        doCycles(serPort, telnet)

    logger.debug("Closing %s" % SerialPort)
    serPort.write(b'TC')
    serPort.close()
    logger.debug("Waiting on upload exit")
    putUploadFile('exit')
    upl.join()
=======

        global CameraIP
        CameraIP = getCameraIP()
        logger.debug("Establishing telnet connection")
        telnet = telnetlib.Telnet(CameraIP)
        waitForCamera(telnet)

        doneCycles += doCycles(serPort, telnet, numCycles - doneCycles)
        if doneCycles >= numCycles:
            break

        logger.error("doCycles fails, restart camera")
        cameraOff(serPort)
        time.sleep(5)

    logger.debug("Complete, shutting down")
    cameraOff(serPort)
>>>>>>> ca7881c543c89f69132bc2575b0985f545b72a22
    return 0

sys.exit(main())
