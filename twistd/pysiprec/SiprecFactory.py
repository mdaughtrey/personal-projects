''' Twisted SiprecFactory class
Persistent (ie, trans-protocol instance) configuration is kept here
'''
from twisted.internet.protocol import ClientFactory
from SiprecProtocol import SiprecProtocol

class SiprecFactory(ClientFactory):
    def buildProtocol(self, addr):
        print "connected"
        return SiprecProtocol()

    def startedConnecting(self, connector):
        print "started connecting"

    def clientConnectionLost(self, connector, reason):
        print "lost connection, %s" % reason

    def clientConnectionFailed(self, connector, reason):
        print "connection failed, %s" % reason

