#!/usr/bin/python

import re
import os
import sys
import ast
import pdb
import urllib
import subprocess

from HTMLParser import HTMLParser
from optparse import OptionParser

class CTOpener(urllib.FancyURLopener):
    version = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36'
urllib._urlopener = CTOpener()

outputfile=''

parser = OptionParser()
parser.add_option('-u', '--url', dest='url')
parser.add_option('-o', '--output', dest='output')
#parser.add_option('-s', '--showname', dest='showname')
parser.add_option('-d', '--directory', dest='directory')
parser.add_option('-v', '--vendor', dest='vendor', default='allmyvideos')
(options, args) = parser.parse_args()

class TheParser(HTMLParser):
    def __init__(self, outputfile = None):
        if outputfile:
            self._outputfile = outputfile
        HTMLParser.__init__(self)

    def callBackOnStartTag(self, lookFor, cb):
        self._lookForStartTag = lookFor 
        self._cbStartTag = cb

    def callBackOnData(self, lookFor, cb):
        self._lookForData = lookFor
        self._cbData = cb

    def recurseAttrs(self, attr):
        if 2 == len(attr) and 'data-iframe' == attr[1][0]:
            src = [elem for elem in attr[1][1].strip().split(' ') if 'src' in elem]
            return self.recurseAttrs(('src', src[0].split('=')[1].replace('"','')))

        if 2 != len(attr):
            for elem in attr:
                if True == self.recurseAttrs(elem):
                    return True
            return False

        (name, value) = attr

        if False == isinstance(name, str) or False == isinstance(value, str):
            return False

        if 'src' != name:
            return False

        if -1 != value.find(self._lookForStartTag):
            if self._cbStartTag:
                self._cbStartTag(value)
                return True
        return False

    def handle_starttag(self, tag, attrs):
        print 'starttag %s' % tag
        if tag != 'b':
            return
        pdb.set_trace()
        self.recurseAttrs(attrs)

    def handle_endtag(self, tag):
        pass

    def handle_data(self, data):
        print 'data %s' % data
        try:
            if '.mp4' in data:
                pdb.set_trace()
                self._extractMP4File(data, self._outputfile)
        except AttributeError:
            return

    def _extractMP4File(self, data, outputfile):
        origData = data
#        if -1 == data.find(self._lookForData):
#            return

        data = data[data.find('{'):data.rfind('}') + 1]
        patterns = [
            ('\n',''),
            ('$(window).width()','""'),
            ('$(window).height()','""'),
            ('true,','"true",'),
            ('false,','"false",')
            ]

        for pattern in patterns:
            fixed = data.replace(pattern[0], pattern[1])
            data = fixed

        jData = ast.literal_eval(data)
        try:
            for elem in jData['playlist'][0]['sources']:
                if -1 != elem['label'].find('720'):
                    mp4File = elem['file']
                    print "Downloading %s to %s" % (mp4File, outputfile)
                    subprocess.call(['wget', mp4File, '-O', outputfile])
                    sys.exit(0)

        except AttributeError:
            print "Invalid JSON data, something in the vendor code has changed"
            print "Original data is %s" % origData
            sys.exit(1)

def repairHtml(toFix):
    patterns = [
        ('</"+e+"script>', '</script>'),
        ('sc"+"ript', 'script'),
        ('</"+"script', '</script'),
        ('<scr"+e+"ipt>', '<script>')
    ]
    toFix = toFix.decode('utf8')
    toFix = toFix.encode('ascii','ignore')
    for pattern in patterns:
        fixed = toFix.replace(pattern[0], pattern[1])
        toFix = fixed

    return fixed


def processPage(url):
    print "Retrieving %s" % url
    page = urllib.urlretrieve(url)
    print "Retrieved %s to %s" % (url, page[0])
    htmlFile = open(page[0])
    htmlData = repairHtml(''.join(htmlFile.readlines()))
    parser = TheParser()

    if -1 != htmlData.find(options.vendor):
        parser.callBackOnStartTag(options.vendor, lambda url: startTagAllMyVideos(url))
        #parser.callBackOnStartTag(options.vendor, lambda url: startTagAllMyVideos(url))
#
#
#    if -1 != htmlData.find('allmyvideos.net'):
#        parser.callBackOnStartTag('allmyvideos.net', lambda url: startTagAllMyVideos(url))
#    elif -1 != htmlData.find('ishared.eu'):
#        parser.callBackOnStartTag('ishared.eu', lambda url: startTagIShared(url))
#    elif -1 != htmlData.find('vidspot.net'):
#        parser.callBackOnStartTag('vidspot.net', lambda url: startTagIShared(url))
    parser.feed(htmlData)

#def mp4Data(data):
#    pass

#def startTagIShared(url):
#    page = urllib.urlretrieve(url)
#    htmlFile = open(page[0])
#    parser = TheParser(outputfile)
#    htmlData = repairHtml(''.join(htmlFile.readlines()))
#    iMp4 = htmlData.find('mp4')
#    iStart = htmlData.rfind('"', 0, iMp4)
#    iEnd = htmlData.find('"', iMp4)
#    mp4File = htmlData[iStart + 1: iEnd]
#    print "Downloading %s to %s" % (mp4File, outputfile)
#    subprocess.call(['wget', mp4File, '-O', outputfile])
#    sys.exit(0)
#    parser.callBackOnData('.mp4', lambda data: mp4Data(data))
#    parser.feed(htmlData)

def startTagAllMyVideos(url):
    pdb.set_trace()
    page = urllib.urlretrieve(url)
    htmlFile = open(page[0])
    parser = TheParser(outputfile)
    fixedData = repairHtml(''.join(htmlFile.readlines()))
#    parser.callBackOnData('.mp4', lambda data: mp4Data(data))
    parser.feed(fixedData)

def main():
    global outputfile
    outputfile = "%s/%s" % (options.directory, options.output)
#    outputfile = "%s/%s/%s-%s.mp4" % (options.directory,
#            options.showname,
#            options.showname,
#            options.output)

    if os.path.isfile(outputfile):
        print "%s exists, skipping" % outputfile
        sys.exit(0)

#outputdir="%s/%s" % (options.directory, options.showname)
#    if not os.path.exists(outputdir):
#        os.mkdir(outputdir)

    processPage(options.url)


main()

