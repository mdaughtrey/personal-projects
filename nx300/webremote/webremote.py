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
argparser.add_argument('--testfile', dest = 'testfile')
argparser.add_argument('--nohup', dest = 'nohup', action='store_true')
argparser.add_argument('--tcp-nodelay', dest = 'tcp_nodelay', action='store_true')
argparser.add_argument('--log-level', dest = 'log_level')
argparser.add_argument('--mode', dest = 'mode', required = True, choices=['shot2link','getinfo','rshot'])

DebugLevels = { 'debug': logging.DEBUG, 'info': logging.INFO, 'error': logging.ERROR, 'warning': logging.WARNING }

args = argparser.parse_args()

addrSelector = {
    'shot2link': { 'addr':'192.168.0.33', 'port': 7788 },
    'getinfo': { 'addr': '192.168.0.33', 'port': 7788 },
    'dlna': { 'addr': '192.168.0.33', 'port': 7788 },
    'rshot': { 'addr': '192.168.0.33', 'port': 7788 },
}


def init():
    pdb.set_trace()
    connInfo = addrSelector[args.mode]
    conn = httplib.HTTPConnection(connInfo['addr'], connInfo['port'])
    headers = {
        'User-Agent': 'SEC_MODE_+1231231234',
        'Connection': 'Close',
        'NTS': 'Alive',
        'HOST-Mac': '02:00:00:00:00:00',
        'HOST-Address': '192.168.0.31',
        'HOST-port': '7788',
        'HOST-PNumber': '+1231231234',
        'Access-Method': 'Manual',
        'CALLBACK': '<http://192.168.0.31:7792/eventCallback>'
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
    pdb.set_trace()
    res = socket.getaddrinfo(addrSelector[args.mode]['addr'], 801, socket.AF_UNSPEC, socket.SOCK_STREAM)[0]
    af, socktype, proto, canonname, sa = res
    sock = socket.socket(af, socktype, proto)
    sock.connect(sa)
    sock.sendall('\r\n'.join(['S2L/1.0 Request',
        'Host: SAMSUNG-S2L',
        'Content-Type: text/xml;charset=utf-8',
        'User-Agent: APP-TYPE',
        'Content-Length: 0',
        'HOST-Mac : 02:00:00:00:00:00',
        'HOST-Address : 192.168.0.31',
        'HOST-port : 1801',
        'HOST-PNumber : +1231231234',
        'Host-Gps : N147396XW266202',
        'Access-Method : manual',
        '']))

    data = sock.recv(2048)
    logger.debug('shot2Link %s' % data)
    sock.close()


def getInformation():
    conn = httplib.HTTPConnection(addrSelector[args.mode]['addr'], 7788)
    headers = {
        'Content-Type': 'text/xml',
        'HOST': 'http://192.168.0.33:7676',
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
    for idx in [1, 2, 3, 4]:
        conn.request('POST', '/smp_%u_' % idx, '', headers)
        response = conn.getresponse()
        print response.status, response.reason, response.getheaders()
        data = response.read()
        print data
    conn.close()

def waitForShots():
    res = socket.getaddrinfo('192.168.0.31', 1801, socket.AF_UNSPEC, socket.SOCK_STREAM)[0]
    af, socktype, proto, canonname, sa = res
    sock = socket.socket(af, socktype, proto)
    if args.tcp_nodelay:
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    sock.bind(sa)
    filenum = 0

    while True:
        logger.debug('Listening')
        sock.listen(1)
        conn, addr = sock.accept()
        accum = []
        logger.debug('Connected from %s',  str(addr))
        contentLength = 0
        cameraHost = ''
        dataIndex = 0

        while contentLength == 0 or len(accum) < contentLength:
            logger.debug('select')
            (rlist, wlist, elist) = select.select([conn], [], [])
            for rsock in rlist:
                logger.debug('read')
                while True: 
                    rdata = rsock.recv(4096)
                    if not rdata:
                        logger.debug('rdata is None, breaking')
                        break
                    logger.debug('Received chunk %u bytes' % len(rdata))
                    accum.extend(rdata)

                    logger.debug('Got %u bytes so far' % len(accum))
                    logger.debug('contentLength %u contentReceived %u' % (contentLength, len(accum)))

                    if 0 == contentLength and len(accum) > 1000:
                        logger.debug('Looking for Content-Length')
                        def find4(arr, idx):
                            if len(arr) < idx + 3:
                                return False
                            for off in [0, 1,2,3]:
                                if arr[idx + off] not in ['\n', '\r']:
                                    return False
                            return True

                        mm = [ii for ii, xx in enumerate(accum[:1000]) if find4(accum, ii)]
                        if not mm:
                            continue
                        dataIndex = mm[0] + 4
                        headers = ''.join(accum[:mm[0] + 4]).split('\r\n')

                        clLine =  filter(lambda xx: 'Content-length:' in xx, headers)
                        logger.debug(clLine)
                        contentLength = int(clLine[0].split(':')[1]) + mm[0] + 4
                        logger.debug('contentLength is %u' % contentLength)

                        chLine =  filter(lambda xx: 'Host:' in xx, headers)
                        logger.debug(chLine)
                        cameraHost = chLine[0].split(':')[1]
                        logger.debug('Camera host is %s' % cameraHost)

                    if len(accum) == contentLength:
                        response = ["S2L/1.0 Result_OK",
                            "Host: %s" % cameraHost,
                            "Content-length: %u" % contentLength,
                            "Authorization: suspend",
                            "Sub-ErrorCode: 0",
                            ""]
                        rsock.sendall('\r\n'.join(response))
                        if args.nohup is not None:
                            logger.debug("Socket close")
                            rsock.close()
                        outfile='nx300_%u.jpg' % filenum
                        logger.debug("Writing to %s" % outfile)
                        open(outfile, 'w').write(''.join(accum[dataIndex:]))
                        filenum += 1
                        break

        logger.debug('Received all content')

def processTestFile(testfile):
    accum = open(testfile).read()

    def find4(arr, idx):
        if len(arr) < idx + 3:
            return False
        for off in [0, 1,2,3]:
            if arr[idx + off] not in ['\n', '\r']:
                return False
        return True

    mm = [ii for ii, xx in enumerate(accum[:1000]) if find4(accum, ii)]
    headerData = accum[:mm[0] + 4].split('\r\n')
    pdb.set_trace()
    clLine =  filter(lambda xx: 'Content-length:' in xx, headers)
    cLength = int(clLine[0].split(':')[1])

def rshotPair():
    res = socket.getaddrinfo('192.168.107.2', 7792, socket.AF_UNSPEC, socket.SOCK_STREAM)[0]
    af, socktype, proto, canonname, sa = res
    sock = socket.socket(af, socktype, proto)
    if args.tcp_nodelay:
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    sock.bind(sa)
    filenum = 0

    while True:
        logger.debug('Listening')
        sock.listen(1)
        conn, addr = sock.accept()
        accum = []
        logger.debug('Connected from %s',  str(addr))
        contentLength = 0
        cameraHost = ''
        dataIndex = 0

        while contentLength == 0 or len(accum) < contentLength:
            logger.debug('select')
            (rlist, wlist, elist) = select.select([conn], [], [])
            for rsock in rlist:
                logger.debug('read')
                while True: 
                    rdata = rsock.recv(4096)
                    if not rdata:
                        logger.debug('rdata is None, breaking')
                        break
                    logger.debug('Received chunk %u bytes' % len(rdata))
                    accum.extend(rdata)

                    logger.debug('Got %u bytes so far' % len(accum))
                    logger.debug('contentLength %u contentReceived %u' % (contentLength, len(accum)))

'''                    
                    if 0 == contentLength and len(accum) > 1000:
                        logger.debug('Looking for Content-Length')
                        def find4(arr, idx):
                            if len(arr) < idx + 3:
                                return False
                            for off in [0, 1,2,3]:
                                if arr[idx + off] not in ['\n', '\r']:
                                    return False
                            return True

                        mm = [ii for ii, xx in enumerate(accum[:1000]) if find4(accum, ii)]
                        if not mm:
                            continue
                        dataIndex = mm[0] + 4
                        headers = ''.join(accum[:mm[0] + 4]).split('\r\n')

                        clLine =  filter(lambda xx: 'Content-length:' in xx, headers)
                        logger.debug(clLine)
                        contentLength = int(clLine[0].split(':')[1]) + mm[0] + 4
                        logger.debug('contentLength is %u' % contentLength)

                        chLine =  filter(lambda xx: 'Host:' in xx, headers)
                        logger.debug(chLine)
                        cameraHost = chLine[0].split(':')[1]
                        logger.debug('Camera host is %s' % cameraHost)

                    if len(accum) == contentLength:
                        response = ["S2L/1.0 Result_OK",
                            "Host: %s" % cameraHost,
                            "Content-length: %u" % contentLength,
                            "Authorization: suspend",
                            "Sub-ErrorCode: 0",
                            ""]
                        rsock.sendall('\r\n'.join(response))
                        if args.nohup is not None:
                            logger.debug("Socket close")
                            rsock.close()
                        outfile='nx300_%u.jpg' % filenum
                        logger.debug("Writing to %s" % outfile)
                        open(outfile, 'w').write(''.join(accum[dataIndex:]))
                        filenum += 1
                        break
'''


modeRouter = {
    'shot2link': lambda: init() or shot2Link() or waitForShots(),
    'getinfo': lambda: init() or getInformation(),
    'rshot': lambda: init() or rshotPair()
}


def main():
    modeRouter[args.mode]()

main()
