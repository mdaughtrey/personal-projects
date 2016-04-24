#!/usr/bin/python

import subprocess
import pdb
import os

class FlashAir():
    AIRPORT='en0'
    SSID='CamSD'
    KEY='12345678'
    IP='192.168.0.222'
    SETAIRPORTPOWER='networksetup -setairportpower '
    GETAIRPORTPOWER='networksetup -getairportpower '
    SETAIRPORTNETWORK='networksetup -setairportnetwork '
    MOUNTWEBDAV="mount_webdav -S -s "

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

    def connectToCard(self):
        result = subprocess.check_output((FlashAir.SETAIRPORTNETWORK + "%s %s %s" % (FlashAir.AIRPORT, FlashAir.SSID, FlashAir.KEY)).split(' '))
        if '' != result:
            return False
        return True

    def mount(self):
        mounted="/Volumes/camsd"
        if False == os.path.isdir(mounted):
            os.mkdir(mounted)

        while False == os.path.isdir(mounted + '/DCIM'):
            result = subprocess.check_output((FlashAir.MOUNTWEBDAV + "http://%s %s" % (FlashAir.IP, mounted)).split(' '))



pdb.set_trace()
sdcard = FlashAir()
sdcard.connect()
sdcard.mount()
sdcard.disconnect()

