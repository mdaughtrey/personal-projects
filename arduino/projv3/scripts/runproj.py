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
import httplib
import re

options = {}
EX_OK = 0
EX_TIMEOUT = 1
EX_ERROR = 2
EX_NOFILES = 3

exitCode = 0

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
fileHandler = RotatingFileHandler(filename = './runproj_log.txt', maxBytes = 10e6, backupCount = 5)
fileHandler.setLevel(logging.DEBUG)
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(lineno)s %(message)s'))
logger = logging.getLogger('runproj')
logger.addHandler(fileHandler)

def Sleep(sleepFor):
    logger.debug("Sleeping for %u seconds" % sleepFor)
    time.sleep(sleepFor)

class FlashAir():
    AIRPORT='en0'
    SSID='CamSD'
    KEY='12345678'
    IP='192.168.1.222'
    SETAIRPORTPOWER='networksetup -setairportpower '
    GETAIRPORTPOWER='networksetup -getairportpower '
    SETAIRPORTNETWORK='networksetup -setairportnetwork '
#    MOUNTWEBDAV="mount_webdav -s -S "
#    MOUNTED="/Volumes/camsd"

    def connect(self):
        self._turnAirportOff()
        Sleep(5)
        self._turnAirportOn()
        retryCount = 0
        while False == self.connectToCard():
            retryCount += 1
            if retryCount > 5:
                logger.error('Gave up after 5 tries')
                return False
            Sleep(10)

        return True
            
    def disconnect(self):
        self._turnAirportOff()

    def _isAirportOn(self):
        try:
            result = subprocess.check_output((FlashAir.GETAIRPORTPOWER + "%s" % FlashAir.AIRPORT).split(' '))
            result = result.split(':')[-1].strip()
            if "On" == result:
                return True
            else:
                return False

        except CalledProcessError as err:
            return False

    def _turnAirportOn(self):
        logger.debug("_turnAirportOn")
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s on" % FlashAir.AIRPORT).split(' '))

    def _turnAirportOff(self):
        pdb.set_trace()
        logger.debug("_turnAirportOff")
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s off" % FlashAir.AIRPORT).split(' '))

    def connectToCard(self):
        logger.info("connectToCard")
        result = subprocess.check_output((FlashAir.SETAIRPORTNETWORK + "%s %s %s" % (FlashAir.AIRPORT, FlashAir.SSID, FlashAir.KEY)).split(' '))
        if '' != result:
            logger.error("Connect to %s failed" % FlashAir.SSID)
            return False

        logger.info("WIFI Connected to %s" % FlashAir.SSID)
        self.conn = httplib.HTTPConnection(FlashAir.IP)
        return True

    def getFiles(self):
        command="/command.cgi?op=100&DIR=/DCIM"
        logger.info("getFiles: Requesting %s" % command)
        retryCount = 0
        while True:
            try:
                self.conn.request("GET", command)
                break
            except:
                retryCount += 1
                if retryCount > 5:
                    logger.error('Giving up')
                    return False
                logger.debug("Retrying")

        response = self.conn.getresponse()
        lines = response.read().split('\r\n')[1:-1]
        logger.info("Requested %s" % command)
        logger.info("%u dirs on card" % len(lines))
        dirNums = {} 
        for line in lines:
            (root, dir, size, attr, fDate, fTime) = line.split(',')
#            elems = line.split(',')
            logger.info(line)
            matched = re.search('(\d\d\d)PHOTO', dir)
            if matched is None:
                continue
            dirNums[(int(fDate) * 86400) + int(fTime)] = matched.groups()[0]
#            dirNums.append(int(matched.groups()[0]))
#        if (1 == len(dirNums.keys())):
            # we don't want to retrieve files until the camera is done with it
#            return None

        dirNum = int(dirNums[min(dirNums.keys())])
        logger.debug("Requesting /command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)
        self.conn.request("GET", "/command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)
        logger.debug("Requested /command.cgi?op=100&DIR=/DCIM/%03uPHOTO" % dirNum)

        response = self.conn.getresponse()
        lines = response.read().split('\r\n')[1:-1]
        if [] == lines:
            return None
        urlList = []
        for line in lines:
            elems = line.split(',')
            matched = re.search('SAM_\d+.JPG', elems[1])
            if matched is None:
                continue
            urlList.append("/DCIM/%sPHOTO/%s" % (dirNum, elems[1]))

        return urlList

    def getFile(self, url):
        try:
            logger.debug("GET %s" % url)
            self.conn.request("GET", url)
            logger.debug("response")
            response = self.conn.getresponse()
            logger.debug("return")
            return response.read()
        except:
            return None

    def deleteFile(self, url):
        self.conn.request("GET", "/upload.cgi?DEL=%s" % url)
        response = self.conn.getresponse()
        logger.debug("deleteFile %s" % response.read())
        return response.read()

    def deleteDir(self, dirname):
        self.conn.request("GET", '/upload.cgi?DEL=%s' % dirname)
        response = self.conn.getresponse()
        logger.debug("deleteDir %s" % response.read())
        return response.read()

class SerialProtocol(LineOnlyReceiver):
    def connectionMade(self):
        logger.info('Connected to serial port')
        self.accumulated = ''
        pdb.set_trace()
        self._waiton = dict()
        self._waiton = {'Init OK': self._initProjector}
        self.sdCard = FlashAir()
        self.sdCard.disconnect()
        self._session = False

    def connectionLost(self, reason):
        logger.info('Disconnected from serial port %s', reason)
        reactor.callFromThread(reactor.stop)

    def lineReceived(self, line):
        logger.debug('lineReceived %s, %d waiton entries' % (line, len(self._waiton)))
        self.accumulated += line
        for kk, vv in self._waiton.iteritems():
            if -1 != self.accumulated.find(kk):
                logger.debug("Triggered on %s", kk)
                self.accumulated = ''
                reactor.callLater(0, vv)

    def send(self, data):
        logger.debug("Sending: %s" % data)
        self.transport.write(data)

    def _generator(self, sequence):
        ii = 0;
        for ll in sequence:
            logger.debug("Generator %d" % ii)
            ii += 1
            ll()
            yield

    def _initProjector(self):
        self._waiton.pop('Init OK', None)
        logger.debug("_initProjector")
        self._session = True

        def setWaiton(self):
            self._waiton = dict()
            self._waiton['Frames Done'] = self._getFileInfo
            self._waiton['Opto int timeout'] = self._stopProjector

        sequence = [
            lambda: self.send(' '),
            lambda: self.send('vc'),
            lambda: self.send("%s[" % options.pretension),
            lambda: self.send({'8mm': 'd', 'super8': 'D'}[options.mode]),
            lambda: self.send("%so" % options.numframes),
            lambda: setWaiton(self),
            lambda: self.send('S')
            ]
        self._slowSequence(sequence)

    def _slowSequence(self, sequence = None):
        logger.debug("_slowSequence")
        if False == hasattr(self, '_ssIter'):
            self._ssIter = self._generator(sequence)

        try:
            next(self._ssIter)
            reactor.callLater(1, self._slowSequence)

        except StopIteration:
            del self._ssIter

    def _stopProjector(self):
        self._waiton = dict()
        logger.debug("Projector stopped")
        sequence = [
            lambda: self.send('c'),
            lambda: self.send('s'),
            lambda: reactor.callLater(0, lambda: self._getFileInfo())
            ]
        global exitCode
        exitCode = EX_TIMEOUT
        self._slowSequence(sequence)

    def _sdConnect(self):
        if False == self.sdCard.connect():
            logger.error("Aborting")
            raise RuntimeError("SD Card Connect")

    def _getSDFiles(self):
        self.urls = self.sdCard.getFiles()
        if False == self.urls:
            logger.error("getFiles failed, aborting")
            raise RuntimeError("getFiles")

        if self.urls is None:
            logger.info("No ready files yet")
            raise RuntimeError("No ready files")

    def _getFileInfo(self):
        logger.debug("_getFileInfo")

        try:
            logger.debug("Camera Off")
            self.send('C')
            Sleep(5)
            logger.debug("Camera On")
            self.send('c')

            self._sdConnect()
            self._getSDFiles()
            self._processSDFiles()

        except RuntimeError as ee:
            global exitCode
            logger.error("Something crapped out badly %s" % ee.message)
            exitCode = EX_ERROR
            self.transport.loseConnection()

    def _processSDFiles(self):
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
            jpg = self.sdCard.getFile(url)
            logger.debug("Got a jpg")
            if jpg is None:
                logger.debug('Skipping %s' % url)
                continue
            targetdir = '%s/%03uPHOTO/' % (options.targetdir, lowestTarget)

            if False == os.path.isdir(targetdir):
                os.mkdir(targetdir)
            filename = os.path.basename(url)
            try:
                open(targetdir + filename, 'w').write(jpg)
            except:
                logger.error("Error reading %s" % url)

            responseText = self.sdCard.deleteFile(url)
            logger.debug("%s moved to %s %s" % (url, targetdir + filename, responseText))

        logger.debug("Disconnecting from SD Card")
        self.sdCard.disconnect()
        if False == options.nocamera:
            logger.debug("Camera off")
            self.send('C') # camera off
            self.send(' ') # reset

        if options.once:
            self.transport.loseConnection()
        if EX_TIMEOUT != exitCode:
            reactor.callLater(5, self._initProjector)
        else:
            logger.info("Projector timed out, exiting")
            self.transport.loseConnection()

def getOptions():
    global options
    args = ()
    parser = OptionParser()
    parser.add_option('-o', '--once', dest='once', action='store_true')
    parser.add_option('-n', '--nocamera', dest='nocamera', action='store_true')
    parser.add_option('-f', '--numframes', dest='numframes')
    parser.add_option('-s', '--startframe', dest='startframe')
    parser.add_option('-e', '--endframe', dest='endframe')
    parser.add_option('-t', '--targetdir', dest='targetdir')
    parser.add_option('-p', '--pretension', dest='pretension')
    parser.add_option('-r', '--servo', dest='servo')
    parser.add_option('-m', '--mode', dest='mode', help='super8|8mm')
    parser.add_option('-l', '--length', dest='filmlength', help='film length (small|large)')
    parser.add_option('-d', '--serdevice', dest='serdev', help='Serial device',
        default='/dev/tty.usbserial-A601KW2O')

    (options, args) = parser.parse_args()

#    for testopt in ['startframe', 'endframe', 'targetdir', 'mode', 'filmlength']:
#        if testopt not in options.__dict__:
#            logger.error('Missing option [%s]' % testopt)
#            parser.print_help()
#            return None

#    return options


def main():
    logger.info('Init')
    getOptions()
    if options is None:
        logger.error('Bad command line options')
        sys.exit(1)

    if False == os.path.isdir(options.targetdir):
        logger.info("Creating %s" % options.targetdir)
        os.mkdir(options.targetdir)

    serdev = SerialPort(SerialProtocol(), options.serdev, reactor, 57600)
    reactor.run()
    return exitCode


sys.exit(main())
