#!/usr/bin/python

from twisted.internet.serialport import SerialPort
from twisted.internet import reactor
from twisted.protocols.basic import LineOnlyReceiver

#import serial
import time
import sys
import pdb
import logging
import os
import shutil
import subprocess
from glob import glob
from optparse import OptionParser
import httplib
import re

options = {}

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
logger = logging.getLogger('runproj')


class FlashAir():
    AIRPORT='en0'
    SSID='CamSD'
    KEY='12345678'
    IP='192.168.0.222'
    SETAIRPORTPOWER='networksetup -setairportpower '
    GETAIRPORTPOWER='networksetup -getairportpower '
    SETAIRPORTNETWORK='networksetup -setairportnetwork '
#    MOUNTWEBDAV="mount_webdav -s -S "
#    MOUNTED="/Volumes/camsd"

    def connect(self):
        if False == self._isAirportOn():
            self._turnAirportOn()
        while False == self.connectToCard():
            time.sleep(5)
            
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
        while True:
            try:
                self.conn.request("GET", "/command.cgi?op=100&DIR=/DCIM")
                break
            except:
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
# initialize projector
        self._waiton = {'trigger': 'Init OK' , 'action': self._startProjector}

    def connectionLost(self):
        logger.info('Disconnected from serial port')

    def lineReceived(self, line):
        logger.debug('lineReceived %s' % line)
        self.accumulated += line
        if None is not self._waiton:
            logger.debug("Waiting on %s", self._waiton['trigger'])
            if -1 != self.accumulated.find(self._waiton['trigger']):
                logger.debug("Triggered")
                self.accumulated = ''
                reactor.callLater(0, self._waiton['action'])

    def _startProjector(self):
        logger.info("Start Projector")
        self._waiton = {'trigger': 'Frames Done', 'action': self._processFrames}
        self.transport.write('45[3oCv') # pretension 45, 10 frames, go, verbose

    def _processFrames(self):
#        pdb.set_trace()
        sdCard = FlashAir()
        sdCard.connect()
        urls = sdCard.getOldestFiles()
        if urls is None:
            logger.info("No ready files yet")
            self._startProjector()
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

#        responseText = sdCard.deleteDir("%03uPHOTO" % lowestTarget)
#        logger.debug(responseText)

        self._startProjector()

#        sdCard.mount()
#        mounted = sdCard.mounted() + "/DCIM"
#        logger.info("processFrames")
#        logger.info('Scanning camera dir %s' % mounted)
#        photoDirs = glob('%s/???PHOTO' % mounted)
#        logger.info('Scan done')
#        photoNums = [int(os.path.basename(ff).replace('PHOTO','')) for ff in photoDirs]
#        photoNums.sort()
#        if 0 == len(photoNums):
#            logger.error("No photos found in %s" % mounted)
#        else:
#            logger.info("Found %s photos in %s" % (len(photoNums), mounted))
#
#        logger.info('Scanning target dir %s' % options.targetdir)
#        targetDirs = glob('%s/???PHOTO' % options.targetdir)
#        logger.info('Scan done')
#        targetNums = [int(os.path.basename(ff).replace('PHOTO', '')) for ff in targetDirs]
#        targetNums.sort()
#
#        if [] == targetDirs:
#            lowestTarget = 100
#        else:
#            lowestTarget = targetNums[-1] + 1
#
#        for srcnum in photoNums:
#            srcdir = '%s/%03uPHOTO/' % (mounted, srcnum)
#            targetdir = '%s/%03uPHOTO/' % (options.targetdir, lowestTarget)
#            try:
#                logger.info("Copying %s to %s" % (srcdir, targetdir))
#                shutil.copytree(srcdir, targetdir)
#                logger.info("Removing %s" % srcdir)
#                shutil.rmtree(srcdir)
#                logger.info("Processing complete")
#            except (IOError, os.error) as why:
#                logging.error(str(why))
#                return
#            lowestTarget += 1
#
##        sdCard.unmount()
#        sdCard.off()

def getOptions():
    global options
    args = ()
    parser = OptionParser()
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

    for testopt in ['startframe', 'endframe', 'targetdir', 'mode', 'filmlength']:
        if testopt not in options.__dict__:
            logger.error('Missing option [%s]' % testopt)
            parser.print_help()
            return None

    return options


def main():
    logger.info('Init')
    options = getOptions()
    if options is None:
        logger.error('Bad command line options')
        sys.exit(1)

    if False == os.path.isdir(options.targetdir):
        logger.info("Creating %s" % options.targetdir)
        os.mkdir(options.targetdir)

    serdev = SerialPort(SerialProtocol(), options.serdev, reactor, 57600)
    reactor.run()

main()

#
#
#time.sleep(2)
#(frameStart, frameEnd) = [int(xx) for xx in sys.argv[1:3]]
#frameCount = frameStart
#
#def swrite(data):
##    print "Serial: %s" % data
#    serdev.write(data)
#
##command = '%u[-' % ptArray[chunkSize * int(frameStart / 1000)]
#
##time.sleep(1)
#print 'Sleep 5'
#time.sleep(5)
#print 'Set pretension %s' % sys.argv[3]
#command = '%s[-' % sys.argv[3]
#swrite(command)
#swrite('t')
#time.sleep(1)
#
#def fPrint(ii): print ii
#def fSwrite(ii): swrite('C')
#def fNext(ii): swrite('n')
#def fSleep1(ii):
#    print 'Sleep 1'
#    time.sleep(1)
##def fUntension(ii): 
##    if ((ii % 100) == 0):
##        swrite('u')
##        time.sleep(1)
#
##funcTable = [ fPrint, fSwrite, fSleep1, fSleep1 ]
#funcTable = [ fPrint, fSwrite, fSleep1, fSleep1, fSleep1, fSleep1, fSleep1 ]
##funcTable = [ fPrint, fNext, fSleep1, fSleep1, fSleep1, fSleep1, fUntension ]
#    
#for ii in range(frameCount, frameEnd):
##    print "%s <- %s -> %s" % (frameCount, ii, frameEnd)
#    for ff in funcTable:
#        ff(ii)
#            #sys.stdout.write("=> %s" % serdev.read(100))
#
#swrite('x')
#serdev.close()
#
##while ((count > 0))
##do
##    echo $count frames remaining
##    if [[ "$TRIPLE" == "" ]]
##    then
##        echo C | tr -d '\r\n'  > $USBDEV
##        sleep 2
##    else
##        echo 3 | tr -d '\r\n'  > $USBDEV
##        sleep 4
##    fi
##    ((count--))
##done
##echo x | tr -d '\r\n'  > $USBDEV
##
##serdev.close()
#
