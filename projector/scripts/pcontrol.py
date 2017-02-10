#!/usr/bin/python

import time
import io
import sys
import pdb
import logging
from logging.handlers import RotatingFileHandler
import os
import argparse
import serial
#import telnetlib
import requests
import tempfile
import re
import sqlite3
import code
import signal
import socket
import select

ctlSession = requests.Session()
camSession = requests.Session()
CtlTimeout = 1
CamTimeout = 2
lastKeepAlive = 0
serPort = None
wait = 1

def killHandler(sig, frame):
    cameraOff(serPort)
    sys.exit(1)

signal.signal(signal.SIGUSR2, lambda sig, frame: code.interact())
signal.signal(signal.SIGINT, killHandler)
#Fifoname = "/var/tmp/pcontrol.fifo"
dbName="/var/tmp/pcontroldb"
#dbLock = threading.Lock()
SerialPort = '/dev/ttyUSB0'
CameraIP = '0.0.0.0'
CameraPort = 9000
DCIMRoot = '/mnt/mmc'
ControllerIP = '192.168.0.18'
ControllerPort = 5000

FormatString='%(asctime)s %(name)s %(levelname)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=FormatString)
logger = logging.getLogger('pcontrol')
fileHandler = logging.FileHandler(filename = './pcontrol.log')
fileHandler.setFormatter(logging.Formatter(fmt=FormatString))
fileHandler.setLevel(logging.DEBUG)
logger.addHandler(fileHandler)

parser = argparse.ArgumentParser()
parser.add_argument('--cycles', dest='cycles', default=10, type=int, help='number of frames to capture')
parser.add_argument('--filmtype', dest='filmtype', required=True, choices=['8mm','s8'], help='film type')
parser.add_argument('--mode', dest='mode', required=True, choices=['gentitle','genmovie','fifotest','upload','pipeline','uploader'])
parser.add_argument('--filename', dest='filename', help='set filename')
parser.add_argument('--noupload', dest='noupload', action='store_true', help='disable upload')
parser.add_argument('--inline', dest='inline', action='store_true')
parser.add_argument('--batch', dest='batch', type=int, default=1)
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

def tcp(data):
    def nbRecv(camSock):
        accum = ''
        while len(accum) < 6 or 'ENDEND' != accum[-6:]:
            logger.debug("select entry")
            while ([], [], []) != select.select([camSock.fileno()], [], [], 5.0):
                logger.debug('select wake')
                rx = camSock.recv(1000000)
                logger.debug('rx %u bytes' % len(rx))
                if '' == rx:
                    break
                accum += rx
                time.sleep(.1)
#                if 'ENDEND'  == accum[-6:]:
#                    return accum[:-6]
#            logger.warn('select timeout')
        return accum[:-6]

    try:
        try:
            logger.debug('creating socket')
            camSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            logger.debug('created, connecting')
            camSock.connect((CameraIP, CameraPort))
            logger.debug('connected')
        except:
            logger.error("Connect failed")
            camSock = None
            return None

        logger.debug("Connected, sending %s" % data)
        try:
            camSock.sendall(data.rstrip('\r\n'))
            logger.debug("TCP Sent")
        except:
            return tcp(data)

        data = nbRecv(camSock)
        camSock.close()
        return data

    except:
        logger.warn("TCP Socket Error")
        raise RuntimeError("FATAL Socket")
        return None

def putUploadFile(filename):
    try:
        conn = sqlite3.connect(dbName)
        cur = conn.cursor()
        cur.execute("INSERT INTO pending (filename) values('%s')" % filename)
        conn.commit()
        conn.close()

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
            response = ctlSession.put(url=hUrl, data=open(filename).read(),
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

def keepAlive():
#    if (time.clock() - lastKeepAlive) < 20.0:
#        return
#    global lastKeepAlive
#    lastKeepAlive = time.clock()
    uheaders = {
    'Host': '%s' % CameraIP,
    'Connection': 'keep-alive',
    'Accept': '*/*',
    'X-Requested-With': 'XMLHttpRequest',
    'Accept-Encoding': 'gzip,deflate,sdch',
    'Accept-Language': 'en-US,en;q=0.8'
    }

    hUrl = 'http://%s/api/v1/input/inject_keep_alive' % CameraIP
    logger.debug("KeepAlive")
    response = camSession.get(hUrl, timeout=CamTimeout, headers=uheaders)
#    response = requests.get(url=hUrl, headers=uheaders)
    logger.debug("KeepAlive Done")
    if not response.ok:
        logger.error("KeepAlive request failed")
    
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

#def telnetWait(telnet, waitFor):
#    pdb.set_trace()
#    tData = telnet.read_until(waitFor, 2000)
#    if tData is None or len(tData) == 0:
#        logger.error("telnet.read_until %s timeout [%s]" % (waitFor, tData))
#        raise RuntimeError('telnetWait timeout')
#
#    return tData

def transferPicture(dir, filename):
    filename = "%s/DCIM/%s/%s" % (DCIMRoot, dir, filename)
    content = tcp("txf %s" % filename)[:-6]
    logger.debug('%s is %u bytes' % (filename, len(content)))

    if False == args.noupload:
        imgfile = tempfile.NamedTemporaryFile(delete = False, prefix='pcntl')
        imgname = imgfile.name
        imgfile.write(content)
        imgfile.close()
        logger.debug("Wrote imagedata to %s" % imgname)
        putUploadFile(imgname)
        if args.inline:
            uploader()

    if 1 == args.batch:
        logger.debug("Deleting %s" % filename)
        tcp("cmd rm %s" % filename)
    return True


def transferPictures():
    logger.debug('transferPictures')
    numAvailable = 0
    while numAvailable < args.batch * 3:
        # wait for at least 3 available images
        time.sleep(0.5)
        tData = tcp("cmd ls %s/DCIM/*/*.JPG\n" % DCIMRoot)
        logger.debug("tData %s" % tData)
        images = [ee for ee in tData.split('\n') if 'PHOTO/SAM' in ee]
        numAvailable = len(images)

    for image in images:
        ee = image.split('/')[-2:]
        keepAlive()
        time.sleep(10)
        if False == transferPicture(ee[0], ee[1]):
            return False
    return True

def doCycles(serial, numCycles):
#    def waitCardOpsTelnet(telnet):
#        logger.debug("Waiting on card ops telnet")
#        telnet.write('/mnt/mmc/moncard.sh\n')
#        rx = ''
#        while True:
#            rx += telnet.read_some()
#            logger.debug("rx %s" % rx)
#            if "Timeout" in rx:
#                break
#        logger.debug("moncard.sh returns %s" % rx)
#        if "Wait " in rx:
#            logger.error("Error waiting on SD card activity")
#
#    def waitCardOpsTcp():
#        logger.debug("Waiting on card ops TCP")
#        result = tcp("/mnt/mmc/moncard.sh", ['Wait', 'Timeout'])
#        if 'Timeout' in result:
#            return True
#        return False

    for ii in xrange(1, numCycles + 1): #, args.batch):
        logger.debug("Frame %u of %u" % (ii, args.cycles))
        time.sleep(20)
        keepAlive()
        continue

#        for bb in xrange(1, args.batch):
        if serialWaitFor(serial, '{OIT:'):
            logger.debug("OIT")
            sys.exit(0)
        serial.write('200a')
        if False == subclick(['down=Super_L','down=Super_R']):
            return ii

#        if False == transferPictures():
#            return ii
        now = time.clock() 
        while True == serial.getCTS():
            if (time.clock() - now) > 3.0:
                logger.error("CTS loop timed out")
                return ii

        if False == subclick(['up=Super_L','up=Super_R']):
            return ii
        serial.write('n')
        transferPictures()
        time.sleep(2)

#        try:
#            if False == (telnet):
#                return ii
#        except RuntimeError as ee:
#            logger.error("RuntimeError %s" % str(ee))
#            return ii

#        if 1 != args.batch:
#            logger.debug("Deleting mages")
#            tcp("cmd rm %s/DCIM/*" % DCIMRoot)

    return ii

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

#def uploaderInit():
#    initDb()
#    if False == args.inline:
#        global upl
#        upl = Process(target = uploader)
#        upl.start()
#        logger.debug("Started uploader")

def genMovie():
    try:
        hargs = (ControllerIP, ControllerPort)
        hUrl = 'http://%s:%u/genmovie' % hargs
#    return urlRetry(url, lambda: ctlSession.get(hUrl, timeout=CtlTimeout))
        logger.debug(hUrl)
        response = ctlSession.get(hUrl, timeout=CtlTimeout)
    except:
        logger.error("HTTP genmovie fail") # , %s" % ee.message)
    return True

def uploadTitleFiles(filenames):
    def uploadFile(filename, mode):
        match = re.match('title(\d+).txt', filename)
        hargs = (ControllerIP, ControllerPort, mode, match.group(1))
        hUrl = 'http://%s:%u/%s?page=%s' % hargs
        try:
            logger.debug(hUrl)
            response = ctlSession.put(url=hUrl, data=open(filename).read(),
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
            time.sleep(5)
            logger.debug("Waiting for router")
            response = requests.get('http://192.168.0.1/wlanAccess.asp')
            if not response.ok:
                continue
            htmlClip = re.search('<td>(\d+\.\d+\.\d+\.\d+)</td><td>nx300</td>', response.content)
            if htmlClip:
                ipAddress = htmlClip.groups(0)[0]
                logger.debug("Camera IP is %s" % ipAddress)
                return ipAddress

def waitForCamera():
    logger.debug("waitForCamera")
    while None == tcp('cmd uname'):
        time.sleep(1)

#    logger.debug("Deleting extraneous camera images")
#    if False == UseTcp:
#        telnet.write("rm -rf %s/DCIM/* > /dev/null\n" % DCIMRoot)
#        try:
#            telnetWait(telnet, 'nx300:/#')
#        except RuntimeError:
#            return False
#    else:
#        result = tcp("cmd rm -rf %s/DCIM/*" % DCIMRoot)

    while True:
        time.sleep(3)
        if True == subclick(['down=Super_L','down=Super_R']):
            time.sleep(0.5)
            subclick(['up=Super_L','up=Super_R'])
            break

        if True == subclick(['up=Super_L','up=Super_R']):
            break

    logger.debug("Camera ready")

def cameraOff(serPort):
    logger.debug("Closing %s" % SerialPort)
    serPort.write(b'TC')
    serPort.close()

def main():
    logger.debug("Init")
    if args.mode in ('gentitle'):
        import glob
        uploadTitleFiles(glob.glob(args.filename))
        return 0
    if 'genmovie' == args.mode:
        genMovie()
        return 0
#    if False == args.noupload:
#        uploaderInit()
    if 'uploader' == args.mode:
        uploader()
        sys.exit(0)
    if 'upload' == args.mode:
        return uploadFile(args.filename)
    if 'pipeline' != args.mode:
        return 1


    numCycles = args.cycles
    doneCycles = 0
    while True:
        global serPort
        global serPort
        serPort = serial.Serial(SerialPort, 57600) # , timeout=1)
        logger.debug("Opening %s" % SerialPort)
        if serPort.isOpen():
            serPort.close()

        serPort.open()
        time.sleep(2)
        serPort.write(' ')
        #portWaitFor(serPort, '{State:Ready}')
        portWaitFor(serPort, 'Reset')
        # Camera on, film type, autotension
        serPort.write(b'c%st' % {'8mm': 'd', 's8': 'D'}[args.filmtype]) 
        serPort.write('n')

        time.sleep(2)
        if False == serPort.isOpen():
            logger.error("Cannot open %s, exiting" % SerialPort)
            return 1

        global CameraIP
        CameraIP = getCameraIP()
        waitForCamera()
        

        doneCycles += doCycles(serPort, numCycles - doneCycles)
        if doneCycles >= numCycles:
            break

        logger.error("doCycles fails, restart camera")
        cameraOff(serPort)
        time.sleep(5)

    logger.debug("Complete, shutting down")
    cameraOff(serPort)
    return 0

sys.exit(main())
