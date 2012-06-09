import psycopg2
import string
import logging
import pdb

class Database:
    def __init__(self, config):
        self.__conn = psycopg2.connect('dbname=%s user=%s'
                % (config.get('database', 'dbname'),
                    config.get('database', 'user')))

        cursor = self.__conn.cursor()
        cursor.execute('SELECT longname FROM station;')
        try:
            pdb.set_trace()
            self.__knownStations = [ name[0] for name in cursor.fetchall()]
        except TypeError:
            self.__knownStations =() 
        cursor.close()


    def __call__(self):
        pass

    def createNewStationRecord(self, station):
        cursor = self.__conn.cursor()
        cursor.execute("INSERT INTO station VALUES('%(state)s', '%(longname)s',\
'%(icao)s', '%(iata)s', %(synop)d, %(latitude)f, '%(longitude)s',\
%(elevation)d,'%(metar)c', '%(nexrad)c', '%(airmet)c', '%(artcc)c', '%(taf)c',\
'%(upperair)c', '%(rawinsonde)c', '%(windprofiler)c', '%(asos)c',\
'%(awos)c', '%(meso)c', '%(human)c', '%(augmented)c', '%(office_wfo)c',\
'%(office_rfc)c', '%(office_ncep)c','%(country)s');"

                % {
                    'state' : station.state,
                    'longname' : string.replace(station.longname, '\'', ' '),
                    'icao' : station.name,
                    'iata' : station.iata,
                    'synop' : station.synop,
                    'latitude' : station.latitude,
                    'longitude' : station.longitude,
                    'elevation' : station.elevation,
                    'metar' :  ('Y' and station.isMetar) or 'N',
                    'nexrad' : ('Y' and station.isNexrad) or 'N',
                    'airmet' : ('Y' and station.isAviationAirmet) or 'N',
                    'artcc' : ('Y' and station.isAviationArtcc) or 'N',
                    'taf' : ('Y' and station.isAviationTaf) or 'N',
                    'upperair' : ('Y' and station.isUpperAir) or 'N',
                    'rawinsonde' : ('Y' and station.isRawInSonde) or 'N',
                    'windprofiler' : ('Y' and station.isWindProfiler) or 'N',
                    'asos' : ('Y' and station.isAutoAsos ) or 'N',
                    'awos' : ('Y' and station.isAutoAwos ) or 'N',
                    'meso' : ('Y' and station.isAutoMeso ) or 'N',
                    'human' : ('Y' and station.isAutoHuman ) or 'N',
                    'augmented' : ('Y' and station.isAutoAugmented ) or 'N',
                    'office_wfo' : ('Y' and station.isOfficeWfo ) or 'N',
                    'office_rfc' : ('Y' and station.isOfficeRfc ) or 'N',
                    'office_ncep' : ('Y' and station.isOfficeNcep ) or 'N',
                    'country' : station.country
                })
        self.__conn.commit()
        cursor.close()


    def updateStation(self, station):
        if not station.name in self.__knownStations:
            self.createNewStationRecord(station)
            logging.getLogger('New station %s' % station.longname)


