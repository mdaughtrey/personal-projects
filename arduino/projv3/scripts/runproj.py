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

exitCode = 0

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
fileHandler = RotatingFileHandler(filename = './runproj_log.txt', maxBytes = 10e6, backupCount = 5)
fileHandler.setLevel(logging.DEBUG)
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(message)s'))
logger = logging.getLogger('runproj')
logger.addHandler(fileHandler)

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
        if False == self._isAirportOn():
            self._turnAirportOn()
        retryCount = 0
        while False == self.connectToCard():
            retryCount += 1
            if retryCount > 5:
                logger.error('Gave up after 5 tries')
                return False
            time.sleep(5)

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
        logger.debug("_turnAirportOff")
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s off" % FlashAir.AIRPORT).split(' '))

    def connectToCard(self):
        result = subprocess.check_output((FlashAir.SETAIRPORTNETWORK + "%s %s %s" % (FlashAir.AIRPORT, FlashAir.SSID, FlashAir.KEY)).split(' '))
        if '' != result:
            logger.error("Connect to %s failed" % FlashAir.SSID)
            return False

        logger.info("WIFI Connected to %s" % FlashAir.SSID)
        self.conn = httplib.HTTPConnection(FlashAir.IP)
        return True

    def getOldestFiles(self):
        logger.info("Requesting /command.cgi?op=100&DIR=/DCIM")
        retryCount = 0
        while True:
            try:
                self.conn.request("GET", "/command.cgi?op=100&DIR=/DCIM")
                break
            except:
                retryCount += 1
                if retryCount > 5:
                    logger.error('Giving up')
                    return False
                logger.debug("Retrying")

        response = self.conn.getresponse()
        lines = response.read().split('\r\n')[1:-1]
        logger.info("Requested /command.cgi?op=100&DIR=/DCIM")
        dirNums = {} 
        for line in lines:
            (root, dir, size, attr, fDate, fTime) = line.split(',')
#            elems = line.split(',')
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
        logger.debug("GET %s" % url)
        try:
            self.conn.request("GET", url)
            response = self.conn.getresponse()
            return response.read()
        except:
            return None

    def deleteFile(self, url):
        self.conn.request("GET", "/upload.cgi?DEL=%s" % url)
        response = self.conn.getresponse()
        return response.read()

    def deleteDir(self, dirname):
        self.conn.request("GET", '/upload.cgi?DEL=%s' % dirname)
        response = self.conn.getresponse()
        return response.read()

#    def unmount(self):
#        rc = subprocess.call((FlashAir.MOUNTWEBDAV + "http://%s %s" % (FlashAir.IP, FlashAir.MOUNTED)).split(' '))
#        logger.debug("mount_webdav rc %d" % rc)
#
#    def mount(self):
#        if False == os.path.isdir(FlashAir.MOUNTED):
#            os.mkdir(FlashAir.MOUNTED)
#
#        while False == os.path.isdir(FlashAir.MOUNTED + '/DCIM'):
#            result = subprocess.check_output((FlashAir.MOUNTWEBDAV + "http://%s %s" % (FlashAir.IP, FlashAir.MOUNTED)).split(' '))
#        logger.info("Mounted %s" % FlashAir.MOUNTED)
#
#    def mounted(self):
#        return FlashAir.MOUNTED


class SerialProtocol(LineOnlyReceiver):
    def connectionMade(self):
        logger.info('Connected to serial port')
        self.accumulated = ''

        self._waiton = dict()
        self._waiton = {'Init OK': self._startSequence}
#        reactor.callLater(5, self._softReset)
        self._session = False
        self._ssgIter = None

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

    def _softReset(self):
        logger.debug("_softReset")
        self.transport.write(' ')
        self.transport.loseConnection()
#        self._waiton['Reset'] = self._startSequence

    def _startSeqGenerator(self):
        logger.debug("_startSeqGenerator")
        def setWaiton(self):
            self._waiton = dict()
            self._waiton['Frames Done'] = self._processFrames
            self._waiton['Opto int timeout'] = self._stopProjector

        if options.nocamera:
            sequence = [
                lambda: self.transport.write('vc'),
                lambda: reactor.callLater(4, self._processFrames())
                ]
        else:
            sequence = [
                lambda: self.transport.write('vc'),
                lambda: self.transport.write("%s[" % options.pretension),
                lambda: self.transport.write({'8mm': 'd', 'super8': 'D'}[options.mode]),
                lambda: self.transport.write("%so" % options.numframes),
                lambda: self.transport.write('S'),
                lambda: setWaiton(self)
                ]

        for ll in sequence:
            ll()
            yield

    def _generator(self, sequence):
        for ll in sequence:
            ll()
            yield

    def _startSequence(self):
        logger.debug("_startSequence")
        self._session = True
        if self._ssgIter is None:
            self._ssgIter = self._startSeqGenerator()

        try:
            next(self._ssgIter)
            reactor.callLater(1, self._startSequence)
            return

        except StopIteration:
            self._ssgIter = None
            pass

    def _slowSequence(self, generator):
        logger.debug("_startSequence")
        if self._ssIter is None:
            self._ssIter = generator()

        try:
            next(self._ssIter)
            reactor.callLater(1, self._slowSequence)
            return

        except StopIteration:
            self._ssIter = None

    def _stopProjector(self):
        logger.debug("Projector stopped")
        #self.transport.loseConnection()
        self.transport.write('c')
        reactor.callLater(2, self._processFrames(EX_TIMEOUT))

    def _processFrames(self, exCode):
        logger.debug("_processFrames")
        global exitCode
        sdCard = FlashAir()
        if False == sdCard.connect():
            logger.error("Aborting")
            exitCode = EX_ERROR
            self.transport.loseConnection()

        urls = sdCard.getOldestFiles()
        if False == urls:
            logger.error("getOldestFiles failed, aborting")
            exitCode = EX_ERROR
            self.transport.loseConnection()

        if urls is None:
            logger.info("No ready files yet")
            exitCode = EX_ERROR
            self.transport.loseConnection()

        logger.info('Scanning target dir %s' % options.targetdir)
        targetdirs = glob('%s/???PHOTO' % options.targetdir)
        logger.info('Scan done')
        targetNums = [int(os.path.basename(ff).replace('PHOTO', '')) for ff in targetdirs]
        targetNums.sort()

        if [] == targetdirs:
            lowestTarget = 100
        else:
            lowestTarget = targetNums[-1] + 1

        for url in urls:
            jpg = sdCard.getFile(url)
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

            responseText = sdCard.deleteFile(url)
            logger.debug("%s moved to %s %s" % (url, targetdir + filename, responseText))

        exitCode = exCode

        if False == options.nocamera:
            logger.debug("Camera off")
            self.transport.write('C') # camera off
            self.transport.write(' ') # reset

        self.transport.loseConnection()

def getOptions():
    global options
    args = ()
    parser = OptionParser()
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
