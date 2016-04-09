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
from glob import glob
from optparse import OptionParser

options = {}

logging.basicConfig(level = logging.DEBUG, format='%(asctime)s %(levelname)s %(lineno)s %(message)s')
logger = logging.getLogger('runproj')

class SerialProtocol(LineOnlyReceiver):
    def connectionMade(self):
        logging.info('Connected to serial port')
        self.accumulated = ''
# initialize projector
        self._waiton = {'trigger': 'Init OK' , 'action': self._startProjector}

    def connectionLost(self):
        logging.info('Disconnected from serial port')

    def lineReceived(self, line):
        logging.debug('lineReceived %s' % line)
        self.accumulated += line
        if None is not self._waiton:
            if -1 != self.accumulated.find(self._waiton['trigger']):
                reactor.callLater(0, self._waiton['action'])

    def _startProjector(self):
        logger.info("Start Projector")
        self._waiton = {'trigger': 'Frames Done', 'action': self._processFrames}
        self.transport.write('45[3oCv') # pretension 45, 10 frames, go, verbose

    def _processFrames(self):
        logger.info("processFrames")
        logger.info('Scanning camera dir %s' % options.cameradir)
        photoDirs = glob('%s/???PHOTO' % options.cameradir)
        logger.info('Scan done')
        photoNums = [int(os.path.basename(ff).replace('PHOTO','')) for ff in photoDirs]
        photoNums.sort()
        logger.info('Scanning target dir %s' % options.targetdir)
        targetDirs = glob('%s/???PHOTO' % options.targetdir)
        logger.info('Scan done')
        targetNums = [int(os.path.basename(ff).replace('PHOTO', '')) for ff in targetDirs]
        targetNums.sort()

        if [] == targetDirs:
            lowestTarget = 100
        else:
            lowestTarget = targetNums[-1] + 1

        for srcnum in photoNums:
            srcdir = '%s/%03uPHOTO/' % (options.cameradir, srcnum)
            targetdir = '%s/%03uPHOTO/' % (options.targetdir, lowestTarget)
            try:
                logger.info("Copying %s to %s" % (srcdir, targetdir))
                shutil.copytree(srcdir, targetdir)
                logger.info("Removing %s" % srcdir)
                shutil.rmtree(srcdir)
                logger.info("Processing complete")
            except (IOError, os.error) as why:
                logging.error(str(why))
                return
            lowestTarget += 1

        self._startProjector()

def getOptions():
    global options
    args = ()
    parser = OptionParser()
    parser.add_option('-s', '--startframe', dest='startframe')
    parser.add_option('-e', '--endframe', dest='endframe')
    parser.add_option('-c', '--cameradir', dest='cameradir')
    parser.add_option('-t', '--targetdir', dest='targetdir')
    parser.add_option('-p', '--pretension', dest='pretension')
    parser.add_option('-r', '--servo', dest='servo')
    parser.add_option('-m', '--mode', dest='mode', help='super8|8mm')
    parser.add_option('-l', '--length', dest='filmlength', help='film length (small|large)')
    parser.add_option('-d', '--serdevice', dest='serdev', help='Serial device',
        default='/dev/tty.usbserial-A601KW2O')

    (options, args) = parser.parse_args()

    for testopt in ['startframe', 'endframe', 'cameradir', 'targetdir', 'mode', 'filmlength']:
        if testopt not in options.__dict__:
            logging.error('Missing option [%s]' % testopt)
            parser.print_help()
            return None

    return options


def main():
    logging.info('Init')
    options = getOptions()
    if options is None:
        logging.error('Bad command line options')
        sys.exit(1)

    if False == os.path.isdir(options.cameradir):
        logger.error('Missing cameradir %s' % options.cameradir)
        return

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
