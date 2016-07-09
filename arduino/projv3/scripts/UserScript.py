#!/usr/bin/python

from LJMsg import *
from LJTypes import *
from UserBase import *
import re
import pdb
import sys
import time
import binascii
import logging
import socket

from twisted.internet.serialport import SerialPort
from twisted.internet import reactor
from twisted.protocols.basic import LineOnlyReceiver
from logging.handlers import RotatingFileHandler

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s %(levelname)s %(module)s %(message)s')
logger = logging.getLogger('userscript')

fileHandler = RotatingFileHandler(filename = '/storage/log_userscript.txt', maxBytes = 10e6, backupCount = 5)
fileHandler.setLevel(logging.DEBUG)
fileHandler.setFormatter(logging.Formatter(fmt='%(asctime)s %(message)s'))
logger.addHandler(fileHandler)

logit = logging.getLogger('userscript')

#EVENTIDE_SIGNATURE = "-2143256773"

class UDPSender():
    def __init__(self, host, port, message):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(message, (host, port))

class IntradoCDR:
    def __init__(self):
        self._parser = self.__parseUnstructured
        self._aliRecords = []
        self._currentAliRecord = []
        self._rawCdr = []
        self._headers = dict()
        self._arrivalTime = 0
        logit.debug ("IntradoCDR init")

    def __parseUnstructured(self, line):
        logit.debug( "__parseUnstructured")
        if self.__isStartOfCdr(line):
            self._parser = self.__parseCdr
        return True

    def __parseCdr(self, line):
        '''Process CDR (non-ALI) info'''
        logit.debug( "__parseCDR")
        line = line.strip()
        if self.__isEndOfCdr(line):
            return False
        matched = re.match('(?P<timestamp>\d\d:\d\d:\d\d\.\d\d\d)\s+\[(?P<type>[^\]]+)]\s+(?P<text>.*)', line)

        if matched:
            messageType = str.strip(matched.group('type'))
            if messageType not in self._headers:
                self._headers[messageType] = []

            self._headers[messageType].append(matched.group('text'))
            self._rawCdr.append(line)
        else:
            for matchText in ['ali  ', 'initial ali  ', 'rebid ali ']:
                if re.match('=====\s+' + matchText, line.lower()):
                    self._parser = self.__parseAli
                    return True;
            logit.error("Error on [%s], aborting processing\r\n" % line);
            #sys.stderr.write("Error on [%s], aborting processing\r\n" % line);
            self.__init__()
            return True

        return True

    def __isStartOfCdr(self, line):
        '''Look for beginning of CDR record'''
        line = line.strip()
        if None != re.search('===== CDR BEGIN ', line):
            return True
        return False

    def __isEndOfCdr(self, line):
        '''Look for end of CDR record'''
        line = line.strip()
        if None != re.search('===== CDR END ', line):
            return True
        return False

    def __parseAli(self, line):
        logit.debug( "__parseAli")
        if self.__isEndOfCdr(line):
            if self._currentAliRecord:
		logit.debug("End of CDR Record, pushing ALI Record")
                self._aliRecords.append(self._currentAliRecord)

            return False

        for matchText in ['ali  ', 'initial ali  ', 'rebid ali ']:
            if re.match('=====\s+' + matchText, line.lower()):
                if self._currentAliRecord:
		    logit.debug("New ALI record, pushing ALI Record")
                    self._aliRecords.append(self._currentAliRecord)
                    self._currentAliRecord = []
                return True

        self._currentAliRecord.append(line)
        return True


    def parseLine(self, line):
        '''Parse CDR text. Returns False on end of record'''
        if self._parser(line):
            return True
        self._arrivalTime = time.time()
        return False

    def arrivalTime(self):
        return self._arrivalTime

    def __name(self, latestAli):
        try:
            return str.strip(latestAli[3][:28])
        except:
            return ""

    def __address1(self, latestAli):
        try:
            return ' '.join(latestAli[4].split())
        except:
            return ""

    def __address2(self, latestAli):
        try:
            return ' '.join(latestAli[5].split())
        except:
            return ""

    def __state(self, latestAli):
        try:
            return latestAli[6].split()[-1]
        except:
            return ""

    def __city(self, latestAli):
        try:
            return ''.join(latestAli[6].split()[:-1])
        except:
            return ""

    def __phone(self, latestAli):
        try:
            phone = ''
            for elem in self._headers['VoIP']:
                matched = re.search('ANI:[^\"]+"(?P<phone>\d+)"', elem)
                if matched:
                    phone = matched.group('phone');
                    break
            return phone
        except:
            return ""

#    def __psap(self, latestAli):
#        pass

    def __latitude(self, latestAli):
        try:
            lat = ''
            matched = re.match('LAT:(?P<latitude>-?[\d\.+-]+)\s+LON:(?P<longitude>-?[\d\.+-]+)', latestAli[9])
            if matched:
                lat = matched.group('latitude')
            return lat
        except:
            return ""

    def __longitude(self, latestAli):
        try:
            long = ''
            matched = re.match('LAT:(?P<latitude>-?[\d\.+-]+)\s+LON:(?P<longitude>-?[\d\.+-]+)', latestAli[9])
            if matched:
                long = matched.group('longitude')
            return long
        except:
            return ""

    def __agent(self):
        try:
            for elem in self._headers['TCI']:
                matched = re.match('CALL CONNECTED BY AGENT = (?P<agent>.*$)', elem)
                if matched:
                    return matched.group('agent')
            return "Unknown"
        except:
            return ""

    def __position(self):
        try:
            for elem in self._headers['TCI']:
                logit.debug("__position testing %s" % elem)
                matched = re.match('.*\s+POS = (?P<position>\S+)', elem)
                if matched:
                    return matched.group('position')
            return "Unknown"
        except:
            return ""

    def intradoCallId(self):
        headerList = self._headers['VoIP']
        if 'AIM' in self._headers:
            headerList += self._headers['AIM']
        for elem in headerList:     #self._headers['VoIP']  + self._headers['AIM']:
            matched = re.match('Incoming Call \(ID: (?P<callid>[^\)]+)', elem)
            if matched:
#                logit.debug('Intrado Call ID for %s is %s' % (elem, matched.group('callid')))
                return matched.group('callid')

        return self.outgoingCallId()

    def outgoingCallId(self):
        headerList = self._headers['VoIP']
#        if 'AIM' in self._headers:
#            headerList += self._headers['AIM']
        #logit.debug('outgoingCallId: header list is %s' % headerList)
        for elem in self._headers['VoIP']:
            matched = re.match('Outgoing Call \(ID: (?P<callid>[^\)]+)', elem)
            if matched:
#                logit.debug('Outgoing Call ID for %s is %s' % (elem, matched.group('callid')))
                return matched.group('callid')
        return ''

    def intradoTrunkId(self):
        try:
            for elem in self._headers['VoIP']:
                matched = re.search('External Call-Identifier\s+(?P<trunkid>\S+)', elem)
                if matched:
                    return matched.group('trunkid')
            return ''
        except:
            return ""

    def externalCallId(self):
        try:
            for elem in self._headers['VoIP']:
                matched = re.search('External Call-Identifier\s+(?P<trunkid>\S+)', elem)
                if matched:
                    return matched.group('trunkid')
            return ''
        except:
            return ""

    def _callerId(self):
        try:
            for elem in self._headers['AIM']:
                matched = re.search('CID: Number "(?P<callerid>[^\"]+)"', elem)
                if matched:
                    return matched.group('callerid')
            return ''
        except:
            return ''

#        [ AIM] CID: Number "3093453713" - Name "GALESBURG CITY "

    def _callingParty(self):
        try:
            for elem in self._headers['AIM']:
                matched = re.search('CID: Number "(?P<callerid>[^\"]+)"\s+ - Name "(?P<name>[^\"]+)"', elem)
                if matched:
                    return matched.group('name')
            return ''
        except:
            return ''

    def isCallMatched(self, callId):
        if callId == self.externalCallId():
            return True
        return False

    def isTrunkMatched(self, trunkId):
        if trunkId == self.intradoTrunkId():
            return True
        return False

    def getCallData(self):
        callData = dict()
        #if 0 != len(self._aliRecords):
        #    latestAli = self._aliRecords.pop()
        #    callData['calling_party'] = self.__name(latestAli)
        #    callData['address'] = self.__address1(latestAli) + self.__address2(latestAli)
        #    callData['state'] = self.__state(latestAli)
        #    callData['city'] = self.__city(latestAli)
        #    callData['phone'] = self.__phone(latestAli)
        #    callData['location'] = '(%s,%s)' % (self.__longitude(latestAli), self.__latitude(latestAli))
        #else:

        callData['calling_party'] = self._callingParty()
        callData['caller_id'] = self._callerId()
        callData['agent'] = self.__agent()
        callData['position'] = self.__position()
        callData['intrado_callid'] = self.intradoCallId()
        callData['trunk_address'] = self.intradoTrunkId()

#        except:
#    raise RuntimeError('incomplete call data')

        logit.debug( "Returning %s" % callData)
        return callData


class IntradoProtocol(LineOnlyReceiver):
    delimiter = '\x0d'
    def __init__(self, parent):
        self._parent = parent

    def connectionMade(self):
        logit.debug("connectionMade")

    def dataReceived(self,data):
        LineOnlyReceiver.dataReceived(self, data)

    def lineReceived(self, line):
	line = line.strip()
        logit.debug( "LineReceived %s" % line)
        self._parent.lineFromSerial(line)


EVENTIDE_SIGNATURE = "-2143256773"
class UserScript(UserBase):
    # Entry point
    def init(self):
        self._directives= dict()
        logit.debug("init")
        self.msg_tx(self.make_reg_config({"classId":6})) # listen for db call starts
#
# Incoming and outgoing CDR parser objects that haven't yet been reconciled
# with call metadata
#
        self.incomingCDR = []    # Key = Intrado Call ID
        self.outgoingCDR = []    # Key = Start time (secs since epoch)
#
# Incoming and outgoing call metadata objects that haven't yet been reconciled
# with CDR data
#
        self.incomingMetadata = dict() # Key = Intrado Call ID
        self.outgoingMetadata = dict() # Key = Start time (secs since epoch)
        self.protocol = IntradoProtocol(self)
        self.readConfigFile()

        self.serial = SerialPort(self.protocol, 1, reactor, baudrate=9600)

        self.parser = IntradoCDR()

        if 'testmeta' in self._directives:
            for line in open(self._directives['testmeta']).readlines():
                (kk, vv) = line.split()
                metadata = dict()
                metadata[kk] = vv
                metadata['guid'] = 'AgUiD'
                self.handle_db_metadata_msg(metadata)

        if 'testcdr' in self._directives:
            for line in open(self._directives['testcdr']).readlines():
                self.lineFromSerial(line)
            logit.debug('test done')


    def readConfigFile(self):
        try:
            iniFile = open('/config/UserScriptConfig.ini', 'r')
            for line in iniFile:
                if '#' == line[0]:
                    continue
                (directive, value) = line.strip("\r\n").split('=')
                self._directives[directive] = value
        except:
            return False


    def __ageCheck(self):
        '''Remove call metadata and CDR data that is more than 5 minutes old'''
        if 'disableagecheck' in self._directives:
            return
        now = time.time()

#        logit.debug("incomingMetadata elements before age check: %s" % len(self.incomingMetadata))
        self.incomingMetadata = dict((kk, vv) for kk, vv in self.incomingMetadata.iteritems()
                if now - vv['arrivaltime'] < 300)
#        logit.debug("incomingMetadata elements after age check: %s" % len(self.incomingMetadata))

#        logit.debug("incomingCDR elements before age check: %s" % len(self.incomingCDR))
        self.incomingCDR = [elem for elem in self.incomingCDR
                if now - elem.arrivalTime() < 300]
#        logit.debug("incomingCDR elements after age check: %s" % len(self.incomingCDR))


    def lineFromSerial(self, line):
        logit.debug (line)
        if True == self._directives.get('saverawcdr', False):
            open('/storage/rawcdr.txt', 'a').write(line)
        if True == self._directives.get('disableparsing', False):
            return

        if True == self.parser.parseLine(line):
            return

        logit.debug ("Parsing Complete")
        self.__ageCheck()
        logit.debug ("Looking up unmatched call info for %s" % self.parser.intradoCallId())

        chanInfo = self.incomingMetadata.get(self.parser.intradoCallId(), None)
        if None != chanInfo:
            logit.debug("Found chanInfo %s in incoming" % chanInfo)
            cdrData = self.parser.getCallData()
#            cdrData['eventide_callid'] = chanInfo.get('eventide_callid', None)
#            if cdrData is None:
#                cdrData['callid'] = chanInfo.get('intrado_callid', None)

            self.sendCallMetadata(cdrData, chanInfo['callguid'])
            del self.incomingMetadata[self.parser.intradoCallId()]
            self.parser = IntradoCDR()
            return

        #else: # search in outgoing calls
        #    chanInfo = self.outgoingMetadata.get(self.parser.outgoingCallId(), None)
        #    if None != chanInfo:

        logit.debug ("Appending parse to pending list")
        self.incomingCDR.append(self.parser)
        self.parser = IntradoCDR()

    def sendCallMetadata(self, callData, guid):
        logit.debug("sendCallMetadata callData %s" % callData)
        encoded = self.encodeMetadata(callData)
        self.msg_tx(self.make_db_metadata_msg({'guid': guid, 'metadataSize': len(encoded), 'metadataBytes':encoded, 'ignoreChannelName': True}))
        # Send ALI block to ljDataSpigot for parsing
	if 0 != len(self.parser._aliRecords):
	    logit.debug("Send via UDP")
	    UDPSender("127.0.0.1", 31415, guid + "\n" + '\n'.join(self.parser._aliRecords.pop()))

    def handle_db_metadata_msg(self, metadata):
        logit.debug("handle_db_metadata_msg(self, metadata)")
        self.__ageCheck()

        guid = metadata['guid']
        if 'testmeta' in self._directives:
            meta_map = metadata
        else:
            mfile = None
            meta_array = metadata['metadataBytes'][:metadata['metadataSize']].split('\0')
            if True == self._directives.get('saverawmeta', False):
                mfile = open('/storage/rawmeta.txt','a')
            for elem in meta_array:
                logit.debug("meta_array %s" % elem)
                if mfile is not None:
                    mfile.write('%s\n' % elem)

            meta_map = dict(meta_array[ii:ii+2] for ii in range(0, len(meta_array) -1, 2))

        #if 'CALLID' in meta_map:
        #    logit.debug("Skipping CALLID metadata message")
        #    return

        for index, parser in enumerate(self.incomingCDR):
            matchedData = dict()

            if 'intrado_callid' in meta_map:
                callId = meta_map['intrado_callid']
                logit.debug("Testing match on callid %s" % callId)
                if parser.isCallMatched(callId):
                    logit.debug("Matched on call %s" % callId)
                    for key in ['eventide_callid', 'intrado_callid']:
                        if key in meta_map:
                            matchedData[key] = meta_map[key]


            if 'trunk_address' in meta_map:
                trunkId = meta_map['trunk_address']
                logit.debug("Testing match on trunkid %s" % trunkId)
                if parser.isTrunkMatched(trunkId):
                    logit.debug("Matched on trunk %s" % trunkId)
                    for key in ['eventide_callid', 'trunk_address']:
                        if key in meta_map:
                            matchedData[key] = meta_map[key]


            if len(matchedData):
                for kk, vv in parser.getCallData().iteritems():
                    matchedData[kk] = vv
                self.sendCallMetadata(matchedData, guid)
                del self.incomingCDR[index]
                return

        if 'intrado_callid' not in meta_map:
            return

#
# No match or no parsers, save the info
#
        logit.debug("Adding unmatched call data to pending list")
        for key in ['intrado_callid', 'eventide_callid', 'trunk_address']:
            if meta_map['intrado_callid'] not in self.incomingMetadata:
                self.incomingMetadata[meta_map['intrado_callid']] = dict()
            if key in meta_map:
                self.incomingMetadata[meta_map['intrado_callid']][key] = meta_map[key]
        self.incomingMetadata[meta_map['intrado_callid']]['arrivaltime'] = time.time()
        self.incomingMetadata[meta_map['intrado_callid']]['callguid'] = guid

        logit.debug("Unmatched call dump:")
        if len(self.incomingMetadata.keys()) > 25:
            logit.debug("Over 50 calls unmatched, not dumping individually")
        else:
            for (kk, vv) in self.incomingMetadata.iteritems():
                logit.debug("%s => %s" % (kk, vv))



