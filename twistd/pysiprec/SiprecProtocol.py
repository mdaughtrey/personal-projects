'''Twisted siprec protocol class'''
from twisted.internet.protocol import Protocol

class SiprecProtocol(Protocol):
    def sendMessage(self, msg):
        self.transport.write(msg)

#    def connectionMade(self):
#        pass
#
#    def connectionLost(self):
#        pass
#
#    def makeConnection(self):
#        pass




