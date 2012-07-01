import sys
import pdb
import logging
import urllib
import os
import fileinput
import re
import pdb
import station
import database

#           1         2         3         4         5         6         7         8
# 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
# CD  STATION         ICAO  IATA  SYNOP   LAT     LONG   ELEV   M  N  V  U  A  C
# AK ADAK NAS         PADK  ADK   70454  51 53N  176 39W    4   X     T          7 US

class Ingest:
    def __init__(self, config):
        self.__config = config
        self._localFilename = config.get('metar', 'stations_txt_local')
        self._log = logging.getLogger('metaringest')
        logging.getLogger('metaringest').debug('init')

    def _blankCheck(self, string):
        if None == re.search('\S', string):
            return '-1' 
        return string 


    def _stationsGenerator(self):
        for line in fileinput.input(self._localFilename):
#            self._log.debug(line)
            line = line.rstrip('\r\n')
            if len(line) != 83:
                continue
            data = { 'CD' : line[0:2],
                'station' : line[3:19].rstrip(' '),
                'icao' : line[20:24], 
                'iata' : line[26:29],
                'synop' : int(self._blankCheck(line[33:37])),
                'latd' : int(self._blankCheck(line[38:41])),
                'latm' : int(self._blankCheck(line[42:44])),
                'latns' : line[44],
                'longd' : int(self._blankCheck(line[47:50])),
                'longm' : int(self._blankCheck(line[51:53])),
                'longew' : line[53],
                'elev' : int(self._blankCheck(line[55:59])),
                'metar' : line[62] != ' ',
                'nexrad' : line[65] != ' ',
                'aviation' : line[68],
                'upper' : line[71],
                'auto' : line[74],
                'office' : line[77],
                'plotting' : int(self._blankCheck(line[79])),
                'country' : line[81:83]}

            newStation = station.Station(data)
            if not newStation():
                continue
            yield newStation

    def __call__(self):
        url = self.__config.get('metar', 'stations_txt_url')

        if not os.path.isfile(self._localFilename):
            self._log.debug("Retrieving %s" % url)
            urllib.urlretrieve(url, self._localFilename)

        #
        # Read the stations file, update the DB if necessary and build the 
        # data query
        #
        stationsDb = database.Database(self.__config)
        for station in self._stationsGenerator():
            stationsDb.updateStation(station)

        return True
