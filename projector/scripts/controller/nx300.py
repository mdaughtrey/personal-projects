class NX300():
    AIRPORT='en0'
    SSID='CamSD'
    KEY='12345678'
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
        while False == self.connectToCard():
            pass


    def disconnect(self):
        self._turnAirportOff()

    def _isAirportOn(self):
        try:
            result = subprocess.check_output((FlashAir.GETAIRPORTPOWER + "%s" % FlashAir.AIRPORT).split(' '))
            result = result.split(':')[-1].strip()
            if "On" == result:
                return True
            else:
                return False

        except CalledProcessError as err:
            return False

    def _turnAirportOn(self):
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s on" % FlashAir.AIRPORT).split(' '))

    def _turnAirportOff(self):
        rc = subprocess.call((FlashAir.SETAIRPORTPOWER + "%s off" % FlashAir.AIRPORT).split(' '))

