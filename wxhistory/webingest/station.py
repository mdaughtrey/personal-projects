class Station:
    def __init__(self, data):
        self._data = data
        self.latitude = data['latd'] + data['latm'] / 60 
        self.longitude = data['longd'] + data['longm'] / 60 
        if data['latns'] == 'S':
             self.latitude = -self.latitude
        if data['longew'] == 'E':
             self.longitude = -self.longitude

        self.isAviationAirmet = data['aviation'] in ['U', 'V'] 
        self.isAviationArtcc = data['aviation'] in ['A']
        self.isAviationTaf = data['aviation'] in ['U', 'T']
        self.isUpperAir = data['upper'] == 'U'
        self.isRawInSonde = data['upper'] == 'X'
        self.isWindProfiler = data['upper'] == 'W'
        self.isAutoAsos = data['auto'] == 'A'
        self.isAutoAwos = data['auto'] == 'W'
        self.isAutoMeso = data['auto'] == 'M'
        self.isAutoHuman = data['auto'] == 'H'
        self.isAutoAugmented = data['auto'] == 'G'
        self.isOfficeWfo = data['office'] == 'F'
        self.isOfficeRfc = data['office'] == 'R'
        self.isOfficeNcep = data['office'] == 'C'

    def __call__(self):
        return True
