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

parser = OptionParser()
parser.add_option('-u', '--url', dest='url')
parser.add_option('-o', '--output', dest='output')
parser.add_option('-s', '--showname', dest='showname')
parser.add_option('-d', '--directory', dest='directory')
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

    def handle_starttag(self, tag, attrs):
#print "starttag tag %s attrs %s" % (tag, attrs)
        if tag != 'iframe':
            return
        for elem in attrs:
            if 'src' != elem[0]:
                continue
            if -1 != elem[1].find(self._lookForStartTag):
#print '%s => %s' % elem
                if self._cbStartTag:
                    self._cbStartTag(elem[1])
#                page = urllib.urlretrieve(elem[1])
                

    def handle_endtag(self, tag):
        pass
#print "endtag tag %s" % tag

    def handle_data(self, data):
#print 'Data1 %s' % data
        try:
            self._extractMP4File(data, self._outputfile)
        except AttributeError:
            return

    def _extractMP4File(self, data, outputfile):
        origData = data
        if -1 == data.find(self._lookForData):
            return

#print "Data2 %s" % data
#        pdb.set_trace()
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
#            if fixed == data:
#                print "Pattern %s replacement failed, something in the vendor code has changed" % pattern[0]
#                print "Original data is %s" % origData
#                sys.exit(1)
            data = fixed

        jData = ast.literal_eval(data)
        try:
            for elem in jData['playlist'][0]['sources']:
                if -1 != elem['label'].find('720'):
                    mp4File = elem['file']
                    outputfile = "%s/%s/%s-%s.mp4" % (options.directory, options.showname, options.showname, outputfile)
                    if os.path.isfile(outputfile):
                        print "%s exists, skipping" % outputfile
                        sys.exit(0)
                    outputdir="%s/%s" % (options.directory, options.showname)
                    if not os.path.exists(outputdir):
                        os.mkdir(outputdir)
                    print "Downloading %s to %s" % (mp4File, outputfile)
                    subprocess.call(['wget', mp4File, '-O', outputfile])
                    sys.exit(0)

        except AttributeError:
            print "Invalid JSON data, something in the vendor code has changed"
            print "Original data is %s" % origData
            sys.exit(1)

def processPage(url):
    print "Retrieving %s" % url
    page = urllib.urlretrieve(url)
    print "Retrieved %s to %s" % (url, page[0])
    htmlFile = open(page[0])
    parser = TheParser()
    parser.callBackOnStartTag('allmyvideos.net', lambda url: startTag(url))
    parser.feed(''.join(htmlFile.readlines()))

def mp4Data(data):
    pdb.set_trace()
    pass


def startTag(url):
#print 'Found startTag %s' % url
    page = urllib.urlretrieve(url)
    htmlFile = open(page[0])
    parser = TheParser(options.output)
#    pdb.set_trace()
    theData = ''.join(htmlFile.readlines())
    fixedData = theData.replace('<scr"+e+"ipt>', '<script>')
    parser.callBackOnData('.mp4', lambda data: mp4Data(data))
    if -1 != fixedData:
        parser.feed(fixedData)
    parser.feed(theData)

def main():
    processPage(options.url)


main()

