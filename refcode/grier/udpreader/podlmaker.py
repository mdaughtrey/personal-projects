#!/usr/bin/python

import struct
import binascii
import hashlib
import pdb

def buildPODL(id, data, header='PODL'):
    params=[]
    for param in (list(header), [id], [len(data)], [ord(yy) for yy in list(data)]):
        params.extend(param)
    msgbin = struct.pack('4cIB%db' % len(data), *params)
    md5 = hashlib.md5()
    md5.update(msgbin)
    msgbin += md5.digest()
    print "MD5 is %s" % ' '.join([hex(ord(elem)) for elem in md5.digest()])
    return msgbin

open('podl_good.bin', 'w').write(buildPODL(0, list('password')))
open('podl_badpass.bin', 'w').write(buildPODL(0xbad1, list('badpassword')))
open('podl_badheader.bin', 'w').write(buildPODL(0xbad2, list('password'), 'PODX'))
data = buildPODL(0, list('password'))
#pdb.set_trace()
#data[-1] = chr(1)
open('podl_badchecksum.bin', 'w').write(data[:-1] + chr(1))
