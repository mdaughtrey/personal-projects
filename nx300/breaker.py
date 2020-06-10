#!/usr/bin/python

from optparse import OptionParser
import pdb
import os
import sys
import struct

def getchunk(binfile, offset, length):
    binfile.seek(offset)
    buffer = binfile.read(length)
    return buffer

def parseMetadata(buffer):
    #(filesize, crc, offset, dest) = struct.unpack('<IIII', buffer)
    return "Size %u CRC %x Offset %u Dest %u" % struct.unpack('<IIII', buffer)

def getVersionInfo(binfile):
    pass

def gendd(meta, binname, outname):
    return "dd skip=%u count=%u if=%s of=%s bs=1" % (meta[2], meta[0], binname, outname)

def main():
    parser = OptionParser('breaker.py [-v] -i binfile')
    parser.add_option('-v', '--verbose', dest='verbose', action='store_true')
    parser.add_option('-i', '--binfile', dest='binfile')
    (options, args) = parser.parse_args()
    if False == os.path.isfile(options.binfile):
        sys.stderr.write('%s not found' % options.binfile)

    binfile = open(options.binfile)

    metavi = getchunk(binfile, 0x40, 0x10)
    metaipl = getchunk(binfile, 0x50, 0x10)
    metapnlbl = getchunk(binfile, 0x60, 0x10)
    metauimage = getchunk(binfile, 0x70, 0x10)
    metaplatform = getchunk(binfile, 0x80, 0x10)
    platforminfo = struct.unpack('<IIII', metaplatform)
    pcachelist = getchunk(binfile, platforminfo[0] + platforminfo[2], 0x10)
    binfile.close()

    if options.verbose:
        print "metavi %s" % parseMetadata(metavi)
        print "metaipl %s" % parseMetadata(metaipl)
        print "metapnlbl %s" % parseMetadata(metapnlbl)
        print "metauimage %s" % parseMetadata(metauimage)
        print "metaplatform %s" % parseMetadata(metaplatform)

    if False == os.path.isfile('vImage.bin'):
        print "%s" % gendd(struct.unpack('<IIII', metavi), options.binfile, 'vImage.bin')
    if False == os.path.isfile('D4_IPL.bin'):
        print "%s" % gendd(struct.unpack('<IIII', metaipl), options.binfile, 'D4_IPL.bin')
    if False == os.path.isfile('D4_PNLBL.bin'):
        print "%s" % gendd(struct.unpack('<IIII', metapnlbl), options.binfile, 'D4_PNLBL.bin')
    if False == os.path.isfile('uImage.bin'):
        print "%s" % gendd(struct.unpack('<IIII', metauimage), options.binfile, 'uImage.bin')
    if False == os.path.isfile('platform.img'):
        print "%s" % gendd(struct.unpack('<IIII', metaplatform), options.binfile, 'platform.img')
    if False == os.path.isfile('pcache.list'):
        print "%s" % gendd(struct.unpack('<IIII', pcachelist), options.binfile, 'pcache.list')

main()

