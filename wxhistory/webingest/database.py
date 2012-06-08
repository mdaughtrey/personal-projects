import psycopg2

class Database:
    def __init__(self, config):
        self.__conn = psycopg2.connect('dbname=%s user=%s'
                % (config.get('database', 'dbname'),
                    config.get('database', 'user')))

        cursor = self.__conn.cursor()
        cursor.execute('SELECT icao FROM station;')
        try:
            self.__knownStations = [ name for (index, name) in cursor.fetchone()]
        except TypeError:
            self.__knownStations =() 
        cursor.close()


    def __call__(self):
        pass

    def createNewStationRecord(self, station):
        cursor = self.__conn.cursor()
        cursor.execute("INSERT INTO station VALUES(%(state)s, %(longname)s,"
                + "%(icao)s, %(iata)s, %(synop)d, %(latitude)f, %(longitude)s,"
                + "%(elevation)d, %(metar)c, %(airmet)c, %(artcc)c, %(taf)c,"
                + "%(upperair)c, %(rawinsonde)c, %(windprofiler)c, %(asos)c,"
                + "%(awos)c, %(meso)c, %(human)c, %(augmented)c, %(office_wfo)c,"
                + "%(office_rfc)c, %(office_ncep)c,%(plot)d, %(country)s);"


                % {
                    'state' : station.state,
                    'longname' : station.longname,
                    'icao' : station.name,
                    'iata' : station.iata,
                    'synop' : station.synop,
                    'latitude' : station.latitude,
                    'longitude' : station.longitude,
                    'elevation' : station.elevation,
                    'metar' :  'Y' and station.isMetar,
                    'airmet' : 'Y' and station.isAviationAirmet,
                    'artcc' : 'Y' and station.isAviationArtcc,
                    'taf' : 'Y' and station.isAviationTaf,
                    'upperair' : 'Y' and station.isUpperAir


        cursor.close()
        pass

    def updateStation(self, station):
        if not station.name in self.__knownStations:
            self.createNewStationRecord(station)


