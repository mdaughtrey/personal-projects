#!/usr/bin/python
'''pysiprec main'''

from twisted.internet import reactor
from twisted.internet.endpoints import TCP4ClientEndpoint
from twisted.python import log

from SiprecProtocol import SiprecProtocol # twisted protocol
from SiprecFactory import SiprecFactory   # twisted factory

print "test"

# classes:
# - configuration/factory
# - logging
# - message builder
# - siprec protocol
# read command line options

def gotProtocol(p):
    p.sendMessage("hello")

def main():
    log.startLogging(open('pysiprec.py.log', 'w'))
    point = TCP4ClientEndpoint(reactor, "localhost", 50505)
    d = point.connect(SiprecFactory())
    d.addCallback(gotProtocol)
    reactor.run()


main()
