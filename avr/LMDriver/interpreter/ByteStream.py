import array
import pdb

class ByteStream:
    def __init__(self):
        self.labelPositions = {}
        self.stream = []
        self.object = array.array('b')

    def _buildSconstMap(self, sconsts):
        alias = 0
        for key, value in sconsts.iteritems():
            sconsts[key] = { 'value' : value, 'alias' : alias }
            alias += 1
        return sconsts

    def svar(self, name, value):
        self.stream.append({ 'svar' : name, 'value' : value })

    def ivar(self, name, value):
        self.stream.append({ 'ivar' : name, 'value' : value })

    def dec(self, name):
        self.stream.append({ 'dec' : name, 'location' : 0 })

    def inc(self, name):
        self.stream.append({ 'inc' : name, 'location' : 0 })

    def jz(self, name, label):
        self.stream.append({ 'jz' : name, 'label' : label })

    def jump(self, label):
        self.stream.append({ 'jump' : label })

    def iemit(self, varname):
        self.stream.append({ 'iemit' : varname })
        pass

    def semit(self, varname):
        self.stream.append({ 'op' : 'semit', 'var' : varname })

    def sleep(self):
        pass

    def label(self, name):
        self.stream.append({ 'labelname' : name, 'location' : 0 })

    def finalize(self, sconsts, svars, ivars):
        [self.object.append(xx) for xx in (0, 0)] # make room for code start offset

        # map sconsts to single byte aliases
        sconsts = self._buildSconstMap(sconsts)
        for name, const in sconsts.iteritems():
            self.object.append(const['alias'])
            self.object.append(len(const['value']) + 1)
            [self.object.append(ord(xx)) for xx in const['value']]

        (address, length) = self.object.buffer_info()
        self.object[0] = (length >> 8) & 0xff
        self.object[1] = length & 0xff

        for cmd in self.stream:
            if 'semit' == cmd['op']:
                self.object.append(ord('e'))

                try:
                    self.object.append(sconsts[cmd['var']]['alias'])

                except KeyError:
                    pass

        self.object.append(ord('!'))
        self.object.tofile(open('obj.bin', 'w'))

# map svars to RAM locations
# map ivars to RAM locations
        pass
