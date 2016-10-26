#!/usr/bin/python

import argparse
import httplib
import pdb
import socket
import select
import logging

logFormat='%(asctime)s %(levelname)s %(name)s %(lineno)s %(message)s'
logging.basicConfig(level = logging.DEBUG, format=logFormat)
logger = logging.getLogger('webremote')
fileHandler = logging.FileHandler('webremote.log')
fileHandler.setLevel(logging.DEBUG)
formatter = logging.Formatter(logFormat)
fileHandler.setFormatter(formatter)
logger.addHandler(fileHandler)

argparser = argparse.ArgumentParser()
argparser.add_argument('--test', dest = 'test')

args = argparser.parse_args()

def init():
    conn = httplib.HTTPConnection('192.168.107.1', 7788)
    headers = {
        'User-Agent': 'SEC_MODE_+1231231234',
        'Connection': 'Close',
        'NTS': 'Alive',
        'HOST-Mac': '02:00:00:00:00:00',
        'HOST-Address': '192.168.0.18',
        'HOST-port': '7788',
        'HOST-PNumber': '+1231231234',
        'Access-Method': 'Manual',
        'CALLBACK': '<http://192.168.0.18:7792/eventCallback>'
    }
    conn.request('HEAD', '/mode/control', None, headers)
    response = conn.getresponse()
    logger.debug('Status %s' % response.status)
    logger.debug('Reason %s' % response.reason)
    logger.debug('Headers %s' % response.getheaders())
    data = response.read()
    logger.debug('Data %s' % data)
    conn.close()

def shot2Link():
    res = socket.getaddrinfo('192.168.107.1', 801, socket.AF_UNSPEC, socket.SOCK_STREAM)[0]
    af, socktype, proto, canonname, sa = res
    sock = socket.socket(af, socktype, proto)
    sock.connect(sa)
    sock.sendall('\r\n'.join(['S2L/1.0 Request',
        'Host: SAMSUNG-S2L',
        'Content-Type: text/xml;charset=utf-8',
        'User-Agent: APP-TYPE',
        'Content-Length: 0',
        'HOST-Mac : 02:00:00:00:00:00',
        'HOST-Address : 192.168.107.2',
        'HOST-port : 1801',
        'HOST-PNumber : +1231231234',
        'Host-Gps : N147396XW266202',
        'Access-Method : manual',
        '']))

    data = sock.recv(2048)
    logger.debug('shot2Link %s' % data)
    sock.close()


def getInformation():
    conn = httplib.HTTPConnection('192.168.107.1', 7788)
    headers = {
        'Content-Type': 'text/xml',
        'HOST': 'http://192.168.107.1:7676',
        'SOAPACTION': 'urn:schemas-upnp-org:service:ContentDirectory:1#GetInfomation'
    }
    body = '''
<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
<s:Body>
    <u:GetInfomation xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
            <GPSINFO>UNKNOWN</GPSINFO>
                </u:GetInformation>
                </s:Body>
                </s:Envelope>
                '''
    conn.request('POST', '/smp_4_', body, headers)
    response = conn.getresponse()
    print response.status, response.reason, response.getheaders()
    data = response.read()
    print data
    conn.close()

def waitForShots():
    res = socket.getaddrinfo('192.168.107.2', 1801, socket.AF_UNSPEC, socket.SOCK_STREAM)[0]
    af, socktype, proto, canonname, sa = res
    sock = socket.socket(af, socktype, proto)
    sock.bind(sa)

    while True:
        sock.listen(1)
        conn, addr = sock.accept()
        logger.debug('Connected from %s',  str(addr))
        contentLength = 0
        contentReceived = 0
        while True:
            (rlist, wlist, elist) = select.select([conn], [], [])
            for rsock in rlist:
                accum = []
                while True: 
                    rdata = rsock.recv(4096)
                    if not rdata:
                        break
                    logger.debug('Received %u bytes' % len(rdata))
                    accum.extend(rdata)

                if 0 == contentLength:
                    try:
                        def find4(arr, idx, val):
                            if len(arr) < idx + 3:
                                return False
                            for off in [1,2,3]:
                                if arr[idx + off] != val + off:
                                    return False
                            return True

                        mm = [ii for ii, xx in enumerate(accum[:1000] find4(accum, ii, xx)]
                        pdb.set_trace()
                        headers = accum[:index]
                        clLine =  filter(lambda xx: 'Content-length:' in xx, headers)
                        cLength = clLine.split(':')[1]
                    except:
                        pass

                print 'received total %u bytes' % len(accum)
                pdb.set_trace()
                open('rsock.bin', 'a').write(accum)

def main():
    init()
    shot2Link()
    waitForShots()
#    getInformation()

main()
