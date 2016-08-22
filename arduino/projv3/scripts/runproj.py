#!/usr/bin/python

from twisted.internet.serialport import SerialPort
from twisted.internet import reactor
from twisted.protocols.basic import LineOnlyReceiver

#import serial
import time
import sys
import pdb
import logging
from logging.handlers import RotatingFileHandler
import os
import shutil
import subprocess
from glob import glob
from optparse import OptionParser
import socket
import httplib
import re

options = {}
EX_OK = 0
EX_TIMEOUT = 1
EX_ERROR = 2
EX_NOFILES = 3

exitCode = 0

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
fileHandler = RotatingFileHandler(filename = './runproj_txt', maxBytes = 10e6, backupCount = 5)
fileHandler.setLevel(logging.DEBUG)
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(lineno)s %(message)s'))
logger = logging.getLogger('runproj')
logger.addHandler(fileHandler)

def Sleep(sleepFor):
    logger.debug("Sleeping for %u seconds" % sleepFor)
    time.sleep(sleepFor)

class FlashAir():
#    AIRPORT='en0'
    SSID='CamSD'
    KEY='12345678'
    IP='192.168.1.222'
    SETAIRPORTPOWER='networksetup -setairportpower '
    GETAIRPORTPOWER='networksetup -getairportpower '
    SETAIRPORTNETWORK='networksetup -setairportnetwork '
    LISTAPS='/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s'

#    def connect(self):
#        self.turnAirportOff()
#        Sleep(5)
#        self.turnAirportOn()
#        Sleep(10)
#        while self.waitingForAP(FlashAir.SSID):
#            Sleep(2)
#        if False == self.connectToCard():
#            raise RuntimeError("FlashAir.connect")
            
    def disconnect(self):
        self.turnAirportOff()

    def waitingForAP(self, ap):
        logger.debug('waitingForAP')
        result = subprocess.check_output((FlashAir.LISTAPS).split(' '))
        for line in result.split('\n'):
            logger.debug(line)
            if ap in line.split():
                return 

        raise RuntimeError("Cannot see %s" % ap)

    def isAirportOn(self):
        try:
            result = subprocess.check_output((FlashAir.GETAIRPORTPOWER + "%s" % options.wifidev).split(' '))
            result = result.split(':')[-1].strip()
            if "On" != result:
                return True
            else:
                return False

        except CalledProcessError as err:
            return False

    def turnAirportOn(self):
        logger.debug("turnAirportOn")
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s on" % options.wifidev).split(' '))
        logging.debug('turnAirportOn %u' % rc)

    def turnAirportOff(self):
        logger.debug("turnAirportOff")
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s off" % options.wifidev).split(' '))
        logging.debug('turnAirportOff %u' % rc)

    def connectToCard(self):
        logger.info("connectToCard")
        result = subprocess.check_output((FlashAir.SETAIRPORTNETWORK + "%s %s %s" % (options.wifidev, FlashAir.SSID, FlashAir.KEY)).split(' '))
        logger.debug(result)
        if '' != result:
            logger.error("Connect to %s failed: %s" % (FlashAir.SSID, result))
            raise RuntimeError("restart")

        logger.info("WIFI Connected to %s" % FlashAir.SSID)

    def getFiles(self):
        command="/command.cgi?op=100&DIR=/DCIM"
        logger.info("getFiles: Requesting %s" % command)
        retryCount = 0
        while retryCount < 5:
#            try:
                retryCount += 1
                self.conn = httplib.HTTPConnection(FlashAir.IP, 80, timeout = 5)
                self.conn.request("GET", command)
                break

#            except httplib.HTTPException as ee:
#                logger.warning("%s, retrying" % ee.message )
#
#            except socket.error as ee:
#                logger.warning("%s, retrying" % ee.message )
#
#            except socket.timeout as ee:
#                logger.warning("%s, retrying" % ee.message )

        if 5 == retryCount:
            logger.error("getFiles failed, abandoning")
            return False

        response = self.conn.getresponse()
        lines = response.read().split('\r\n')[1:-1]
        logger.info("Requested %s" % command)
        logger.info("%u dirs on card" % len(lines))
        self.dirNums = [] 
        for line in lines:
            (root, dir, size, attr, fDate, fTime) = line.split(',')
#            elems = line.split(',')
            logger.info(line)
            matched = re.search('(\d\d\d)PHOTO', dir)
            if matched is None:
                continue
            self.dirNums.append(matched.groups()[0])
#            self.dirNums[(int(fDate) * 86400) + int(fTime)] = matched.groups()[0]
#            dirNums.append(int(matched.groups()[0]))
        if 0 == len(self.dirNums):
            return None

        urlList = []
        for dirNum in sorted(self.dirNums):
#            dirNum = self.dirNums[ii]
            logger.debug("Requesting /command.cgi?op=100&DIR=/DCIM/%sPHOTO" % dirNum)
            self.conn.request("GET", "/command.cgi?op=100&DIR=/DCIM/%sPHOTO" % dirNum)
            logger.debug("Requested /command.cgi?op=100&DIR=/DCIM/%sPHOTO" % dirNum)

            response = self.conn.getresponse()
            lines = response.read().split('\r\n')[1:-1]
            if [] == lines:
                continue
            for line in lines:
                elems = line.split(',')
                matched = re.search('SAM_\d+.JPG', elems[1])
                if matched is None:
                    continue
                urlList.append("/DCIM/%sPHOTO/%s" % (dirNum, elems[1]))

        return urlList

    def getFile(self, url):
        try:
            self.conn = httplib.HTTPConnection(FlashAir.IP, 80, timeout = 5)
            logger.debug("GET %s" % url)
            self.conn.request("GET", url)
            #logger.debug("response")
            response = self.conn.getresponse()
            #logger.debug("return")
            return response.read()
        except:
            return None

    def deleteFile(self, url):
        retryCount = 0
        while retryCount < 3:
            try:
                self.conn.request("GET", "/upload.cgi?DEL=%s" % url)
                response = self.conn.getresponse()
                logger.debug("deleteFile %s" % response.read())
                return response.read()

            except socket.timeout as ee:
                logger.warn("%s, resetting HTTP connection" % ee.message)
                self.conn = httplib.HTTPConnection(FlashAir.IP, 80, timeout = 5)
                retryCount += 1

        return "socket timed out"

    def clearCard(self):
        self.conn = httplib.HTTPConnection(FlashAir.IP, 80, timeout = 5)
        for dirNum in self.dirNums:
#            dirNum = self.dirNums[ii]
            self.deleteDir("%sPHOTO" % dirNum)

    def deleteDir(self, dirname):
        retrycount = 0
        for retry in range(0, 3):
            self.conn.request("GET", '/upload.cgi?DEL=/DCIM/%s' % dirname)
            response = self.conn.getresponse()
            status = response.read()
            logger.debug("deleteDir %s: %s" % (dirname, status))
            if 'SUCCESS' == status:
                return status

    def processFiles(self):
        try:
            self.urls = self.getFiles()
        except socket.timeout as ee:
            logger.error("socket timeout %s" % ee.message)
            raise RuntimeError("restart")

        if False == self.urls:
            logger.error("getFiles failed, aborting")
            raise RuntimeError("getFiles")

        if self.urls is None:
            logger.info("No ready files yet")
            return

        logger.info('Scanning target dir %s' % options.targetdir)
        targetdirs = glob('%s/???PHOTO' % options.targetdir)
        logger.info('Scan done')
        targetNums = [int(os.path.basename(ff).replace('PHOTO', '')) for ff in targetdirs]
        targetNums.sort()

        if [] == targetdirs:
            lowestTarget = 100
        else:
            lowestTarget = targetNums[-1] + 1

        logger.debug("Found %u files" % len(self.urls))
        for url in self.urls:
            #pdb.set_trace()
            jpg = self.getFile(url)
            #logger.debug("Got a jpg")
            if jpg is None:
                logger.debug('Null file, skipping %s' % url)
                continue
            targetdir = '%s/%03uPHOTO/' % (options.targetdir, lowestTarget)

            if False == os.path.isdir(targetdir):
                os.mkdir(targetdir)
            filename = os.path.basename(url)
            try:
                open(targetdir + filename, 'w').write(jpg)
            except:
                logger.error("Error reading %s" % url)

            responseText = self.deleteFile(url)
            logger.debug("%s moved to %s %s" % (url, targetdir + filename, responseText))

class SerialProtocol(LineOnlyReceiver):
    def connectionMade(self):
        logger.info('Connected to serial port')
        self.accumulated = ''
        self.transport.setDTR(False)
        Sleep(1)
        self.transport.setDTR(True)
        self.waiton = dict()
        self.waiton = {"{State:Ready}" : self.initProjector1}
        self.sdCard = FlashAir()
        self.sdCard.disconnect()
        self.currentSlowSeq = None
        self.slowSeqInterval = 2

    def connectionLost(self, reason):
        logger.info('Disconnected from serial port %s', reason)
        reactor.callFromThread(reactor.stop)

    def lineReceived(self, line):
        logger.debug('lineReceived %s, %d waiton entries' % (line, len(self.waiton)))
        self.accumulated += line
        for kk, vv in self.waiton.iteritems():
            if -1 != self.accumulated.find(kk):
                logger.debug("Triggered on %s", kk)
                self.accumulated = ''
                reactor.callLater(0, vv)

    def send(self, data):
        logger.debug("Sending: %s" % data)
        self.transport.write(data)

    def generator(self, sequence):
        ii = 0;
        if sequence is None:
            pdb.set_trace()
        try:
            for ll in sequence:
                ii += 1
                if None != ll.__doc__:
                    logger.debug('Generator executing %u:%s' % (ii, ll.__doc__))
                else:
                    logger.debug('Generator executing %u:%s' % (ii, ll.__name__))
                ll()
                yield

        except TypeError as tt:
            logger.error(tt.message)

        except RuntimeError as ee:
            logger.error("%s at generator step %u" % (ee.message, ii))
            raise

    def initProjector1(self):
        self.waiton.pop('{State:Ready}', None)
        logger.debug("initProjector1")
        self.waiton['{pt:'] = self.initProjector2

        sequence = [
            lambda: self.send(' '),
            lambda: self.send('vt')  # autotension
            ]
        self.slowSequence(sequence)

    def initProjector2(self):
        self.waiton.pop('{pt:}', None)
        logger.debug("initProjector2")

        def setWaiton(self):
            self.waiton = dict()
            self.waiton['{Remaining:0}'] = lambda: self.getFileInfo(last = False)
            self.waiton['Opto int timeout'] = self.stopProjector

        sequence = [
            lambda: self.setSSInterval(2),
            lambda: self.send('c'),
            lambda: self.send({'8mm': 'd', 'super8': 'D'}[options.mode]),
            lambda: self.send("%so" % options.numframes),
            lambda: setWaiton(self),
            lambda: self.send('S')
            ]
        self.slowSequence(sequence)

    def setSSInterval(self, interval):
        self.slowSeqInterval = interval

    def slowSequence(self, sequence = None):
        logger.debug("slowSequence, sequence is %s" % sequence)
        if sequence is not None:
            self.currentSlowSeq = sequence
            logger.debug("self.currentSlowSeq %s" % self.currentSlowSeq)
            self.ssIter = self.generator(sequence)

        if False == hasattr(self, 'ssIter') or self.ssIter is None:
            return

        try:
            next(self.ssIter)
            reactor.callLater(self.slowSeqInterval, self.slowSequence)

        except StopIteration:
            del self.ssIter
            del self.currentSlowSeq

        except RuntimeError as ee:
            del self.ssIter
            if 'restart' == ee.message and self.currentSlowSeq is not None:
                logger.info("Restarting sequence")
#                self.ssITer = self.generator(self.currentSlowSeq)
                reactor.callLater(2, self.slowSequence(self.currentSlowSeq))
            else:    
                pdb.set_trace()
            

    def stopProjector(self):
        reactor.callLater(0, lambda: self.getFileInfo(last = True))
#        self.waiton = dict()
#        logger.debug("Projector stopped")
#        sequence = [
#            lambda: self.send('c'),
#            lambda: self.send('s'),
#            lambda: reactor.callLater(0, lambda: self.getFileInfo(last = True))
#            ]
#        global exitCode
#        exitCode = EX_TIMEOUT
#        self.slowSequence(sequence)

#    def findSDFiles(self):
#        self.urls = self.sdCard.getFiles()
#        if False == self.urls:
#            logger.error("getFiles failed, aborting")
#            raise RuntimeError("getFiles")
#
#        if self.urls is None:
#            logger.info("No ready files yet")
#            raise RuntimeError("No ready files")

    def getFileInfo(self, last = False):
        logger.debug("getFileInfo")

        def testLast(self, last):
            if False == last:
                reactor.callLater(1, self.initProjector1)
            else:
                global exitCode
                exitCode = EX_TIMEOUT
                self.transport.loseConnection()

        self.slowSequence([
            lambda: self.setSSInterval(2),
            lambda: self.send(' '),
            lambda: self.send('c'),
            lambda: self.setSSInterval(5),
            self.sdCard.turnAirportOff,
            self.sdCard.turnAirportOn,
            lambda: self.setSSInterval(10),
            lambda: self.sdCard.waitingForAP(FlashAir.SSID),
            self.sdCard.connectToCard,
            self.sdCard.processFiles,
            lambda: self.setSSInterval(2),
            self.sdCard.clearCard,
            self.sdCard.disconnect,
            lambda: testLast(self, last)
            ])

#            except httplib.HTTPException as ee:
#                logger.warning("%s, retrying" % ee.message )
#
#            except socket.error as ee:
#                logger.warning("%s, retrying" % ee.message )
#
#            except socket.timeout as ee:
#                logger.warning("%s, retrying" % ee.message )
#
#            except RuntimeError as ee:
#                logger.error("Something crapped out badly %s" % ee.message)
#                retryCount += 1
#                if retryCount > 3:
#                    global exitCode
#                    exitCode = EX_ERROR
#                    logging.error("Giving up")
#                    self.transport.loseConnection()
#
#    def processSDFiles(self):
#        self.sdCard.processFiles()
#        logger.debug("Disconnecting from SD Card")
#        self.sdCard.disconnect()
#        logger.debug("Camera off")
#        self.send('C') # camera off
#        self.send(' ') # reset
#
#        if options.once:
#            self.transport.loseConnection()
#        if EX_TIMEOUT != exitCode:
#            reactor.callLater(5, self.initProjector)
#        else:
#            logger.info("Projector timed out, exiting")
#            self.transport.loseConnection()

def getOptions():
    global options
    args = ()
    parser = OptionParser()
    parser.add_option('-o', '--once', dest='once', action='store_true')
    parser.add_option('-x', '--transferonly', dest='transferonly', action='store_true')
    parser.add_option('-f', '--numframes', dest='numframes')
    parser.add_option('-s', '--startframe', dest='startframe')
    parser.add_option('-e', '--endframe', dest='endframe')
    parser.add_option('-t', '--targetdir', dest='targetdir')
    parser.add_option('-p', '--pretension', dest='pretension')
    parser.add_option('-r', '--servo', dest='servo')
    parser.add_option('-m', '--mode', dest='mode', help='super8|8mm')
    parser.add_option('-l', '--length', dest='filmlength', help='film length (small|large)')
    parser.add_option('-w', '--wifidev', dest='wifidev', help='wifi device')
    parser.add_option('-d', '--serdevice', dest='serdev', help='Serial device',
        default='/dev/tty.usbserial-A601KW2O')

    (options, args) = parser.parse_args()

#    for testopt in ['startframe', 'endframe', 'targetdir', 'mode', 'filmlength']:
#        if testopt not in options.__dict__:
#            logger.error('Missing option [%s]' % testopt)
#            parser.print_help()
#            return None

#    return options

class TransferOnly(SerialProtocol):
    def _init__self():
        super(TransferOnly, self).__init__()

    def initProjector1(self):
        self.waiton.pop('{State:Ready}', None)
        logger.debug("TransferOnly")
        self.getFileInfo()

    def getFileInfo(self, last = False):
        logger.debug("getFileInfo")

        self.slowSequence([
            lambda: self.setSSInterval(2),
            lambda: self.send(' '),
            lambda: self.send('c'),
            lambda: self.setSSInterval(5),
            self.sdCard.turnAirportOff,
            self.sdCard.turnAirportOn,
            lambda: self.setSSInterval(10),
            lambda: self.sdCard.waitingForAP(FlashAir.SSID),
            self.sdCard.connectToCard,
            self.sdCard.processFiles,
            lambda: self.setSSInterval(2),
            self.sdCard.clearCard,
            self.sdCard.disconnect
            ])

#def getSDFiles():
#    sdCard = FlashAir()
##    pdb.set_trace()
#    try:
#        sdCard.connect()
#        sdCard.processFiles()
#        sdCard.disconnect()
#
#    except RuntimeError as ee:
#        logger.error(ee.message)


def main():
    logger.info('Init')
    getOptions()
    if options is None:
        logger.error('Bad command line options')
        sys.exit(1)

    if False == os.path.isdir(options.targetdir):
        logger.info("Creating %s" % options.targetdir)
        os.mkdir(options.targetdir)

    if True == options.transferonly:
        #getSDFiles()
        serdev = SerialPort(TransferOnly(), options.serdev, reactor, 57600)
    else:
        serdev = SerialPort(SerialProtocol(), options.serdev, reactor, 57600)
    reactor.run()

    return exitCode


sys.exit(main())
