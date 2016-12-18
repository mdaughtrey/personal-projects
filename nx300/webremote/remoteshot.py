#!/usr/bin/python

import httplib
import time
import pdb
import argparse
import os
import logging
import BeautifulSoup
import telnetlib

CameraIP='192.168.0.33'
ShutterDelay=1
TelnetRoot='/mnt/mmc/'

logFormat='%(asctime)s %(levelname)s %(name)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=logFormat)
logger = logging.getLogger('remoteshot')
fileHandler = logging.FileHandler('remoteshot.log')
fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter(logFormat)
fileHandler.setFormatter(formatter)
logger.addHandler(fileHandler)

argparser = argparse.ArgumentParser()
argparser.add_argument('--frames', dest='numframes', type=int, default=1000, help='Capture N frames')
argparser.add_argument('--dry-run', dest='dryrun', action='store_true', help='dry run')
#argparser.add_argument('--log-level', dest = 'log_level')
#argparser.add_argument('--mode', dest = 'mode', required = True, choices=['shot2link','getinfo','rshot'])

#DebugLevels = { 'debug': logging.DEBUG, 'info': logging.INFO, 'error': logging.ERROR, 'warning': logging.WARNING }

args = argparser.parse_args()

def click(delay = ShutterDelay):
    if True == args.dryrun:
        logger.debug("Simulated click")
        time.sleep(delay)
        return

    conn = httplib.HTTPConnection(CameraIP, 80)
    def subclick(cmds):
        for cmd in cmds:
            url = '/api/v1/input/inject?key%s' % cmd
            conn.request('GET', url)
            response = conn.getresponse()
            data = response.read()
    subclick(['down=Super_L','down=Super_R'])
    time.sleep(delay)
    subclick(['up=Super_L','up=Super_R'])

def transferPicture(dir, filename, telnet):
    conn = httplib.HTTPConnection(CameraIP, 80)
    fileurl = '/DCIM/%s/%s' % (dir, filename)
    fileurl = fileurl.encode('ascii', 'ignore')
    conn.request('GET', fileurl)
    response = conn.getresponse()
    logger.debug('Status %s' % response.status)
    if 200 != response.status:
        logger.error("Status %u getting %s" % (response.status, fileurl))
        return
    data = response.read()
    open('%s' % filename, 'w').write(data)
    logger.debug("Downloaded %s, %u bytes" % (fileurl, len(data)))
    logger.debug("Deleting %s/%s" % (TelnetRoot, fileurl))
    telnet.write("rm %s/%s\n" % (TelnetRoot, fileurl))
    telnet.read_very_lazy()


def getPictures():
    conn = httplib.HTTPConnection(CameraIP, 80)
    headers = {
    'Host': CameraIP,
    'Connection': 'keep-alive',
    'Upgrade-Insecure-Requests': 1,
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.95 Safari/537.36',
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
    'Referer': 'http://%s/' % CameraIP,
    'Accept-Encoding': 'gzip, deflate, sdch',
    'Accept-Language': 'en-US,en;q=0.8'
    }
    
    conn.request('GET', '/DCIM/') #, None, headers)
    response = conn.getresponse()
    #logger.debug('Status %s' % response.status)
    #logger.debug('Reason %s' % response.reason)
    #logger.debug('Headers %s' % response.getheaders())
    data = response.read()
    #logger.debug('Data %s' % data)
    bs=BeautifulSoup.BeautifulSoup(data)
    dirtags=[ee.string for ee in bs.findAll('a') if 'PHOTO' in ee.string]
    for dir in dirtags:
        conn = httplib.HTTPConnection(CameraIP, 80)
        conn.request('GET', '/DCIM/%s' % dir)
        response = conn.getresponse()
        fileInfo = response.read()
        bs=BeautifulSoup.BeautifulSoup(fileInfo)
        fileTags=[ee.string for ee in bs.findAll('a') if '.JPG' in ee.string]
        conn.close()
        telnet = telnetlib.Telnet(CameraIP)
        telnet.read_until('nx300:/#')
        for file in fileTags:
            transferPicture(dir, file, telnet)

    conn.close()

def main():



    logger.debug("Capturing %u frames" % args.numframes)
    for ff in xrange(0, args.numframes):
        logger.debug("Frame %u" % ff)
        click(ShutterDelay)
        getPictures()

main()
