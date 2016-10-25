from multiprocessing import Process
import threading
import subprocess

class NX300():
    AIRPORT='en0'
    SSID='AP_SSC_NX300-0-0D:D0:A1'
    KEY=''
    IP='192.168.0.222'
    SETAIRPORTPOWER='networksetup -setairportpower '
    GETAIRPORTPOWER='networksetup -getairportpower '
    SETAIRPORTNETWORK='networksetup -setairportnetwork '

    def __init__(self, logger, fileman):
        self._logger = logger
        self._fileman = fileman

    def connect(self):
        if False == self._isAirportOn():
            self._turnAirportOn()
        while False == self.remoteConnect():
            pass

    def disconnect(self):
        self._turnAirportOff()

    def remoteConnect(self):
        logger.info("remoteConnect")
        result = subprocess.check_output((NX300.SETAIRPORTNETWORK + "%s %s %s" % (options.wifidev, NX300.SSID, NX300.KEY)).split(' '))
        logger.debug(result)
        if '' != result:
            logger.error("Connect to %s failed: %s" % (NX300.SSID, result))
            raise RuntimeError("restart")

        logger.info("WIFI Connected to %s" % NX300.SSID)

    def _isAirportOn(self):
        try:
            result = subprocess.check_output((NX300.GETAIRPORTPOWER + "%s" % NX300.AIRPORT).split(' '))
            result = result.split(':')[-1].strip()
            if "On" == result:
                return True
            else:
                return False

        except CalledProcessError as err:
            return False

    def _turnAirportOn(self):
        rc = subprocess.call((NX300.SETAIRPORTPOWER + "%s on" % NX300.AIRPORT).split(' '))

    def _turnAirportOff(self):
        rc = subprocess.call((NX300.SETAIRPORTPOWER + "%s off" % NX300.AIRPORT).split(' '))

