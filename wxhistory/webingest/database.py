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
            self.__knownStations = [ name[0] for name in cursor.fetchall()]
        except TypeError:
            self.__knownStations =() 
        cursor.close()


    def __call__(self):
        pass

    def createNewStationRecord(self, station):
        toYN = { True: 'Y', False: 'N' }
        cursor = self.__conn.cursor()
        cursor.execute("INSERT INTO station VALUES(%(state)s, %(longname)s,\
%(icao)s, %(iata)s, %(synop)s, %(latitude)s, %(longitude)s,\
%(elevation)s,%(metar)s, %(nexrad)s, %(airmet)s, %(artcc)s, %(taf)s,\
%(upperair)s, %(rawinsonde)s, %(windprofiler)s, %(asos)s,\
%(awos)s, %(meso)s, %(human)s, %(augmented)s, %(office_wfo)s,\
%(office_rfc)s, %(office_ncep)s,%(country)s);",
    
                     {
                        'state' : station.state,
                        'longname' : station.longname,
                        'icao' : station.name,
                        'iata' : station.iata,
                        'synop' : station.synop,
                        'latitude' : station.latitude,
                        'longitude' : station.longitude,
                        'elevation' : station.elevation,
                        'metar' :  toYN[station.isMetar],
                        'nexrad' : toYN[station.isNexrad],
                        'airmet' : toYN[station.isAviationAirmet],
                        'artcc' : toYN[station.isAviationArtcc],
                        'taf' : toYN[station.isAviationTaf],
                        'upperair' : toYN[station.isUpperAir],
                        'rawinsonde' : toYN[station.isRawInSonde],
                        'windprofiler' : toYN[station.isWindProfiler],
                        'asos' : toYN[station.isAutoAsos],
                        'awos' : toYN[station.isAutoAwos],
                        'meso' : toYN[station.isAutoMeso],
                        'human' : toYN[station.isAutoHuman],
                        'augmented' : toYN[station.isAutoAugmented], 
                        'office_wfo' : toYN[station.isOfficeWfo],
                        'office_rfc' : toYN[station.isOfficeRfc],
                        'office_ncep' : toYN[station.isOfficeNcep],
                        'country' : station.country
                    })
        self.__conn.commit()
        cursor.close()



    def updateStation(self, station):
        if not station.longname in self.__knownStations:
            logging.getLogger('database').debug("Add new longname %s" % station.longname)
            self.createNewStationRecord(station)


