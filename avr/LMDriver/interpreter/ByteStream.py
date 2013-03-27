import array
import pdb

class ByteStream:
    def __init__(self):
        self.labels = {} 
        self.stream = []
        self.object = array.array('B')

    def _buildVarAliases(self, varMap, aliasBase):
        for key, value in varMap.iteritems():
            varMap[key] = { 'value' : value, 'alias' : aliasBase }
            aliasBase += 1
        return varMap

    def _mapIVarLocations(self, varMap):
        ramLocation = 0
        for key, value in varMap.iteritems():
            varMap[key]['location'] = ramLocation
            ramLocation += 2
        return varMap

    def svar(self, name, value):
        self.stream.append({ 'op' : 'svar', 'var' : name, 'value' : value })

    def ivar(self, name, value):
        self.stream.append({ 'op' : 'ivar', 'name' : name, 'value' : value })

    def dec(self, name):
        self.stream.append({ 'op' : 'dec', 'value' : 0 })

    def inc(self, name):
        self.stream.append({ 'op' : 'inc', 'value' : 0 })

    def jz(self, name, label):
        self.stream.append({ 'jz' : name, 'label' : label })

    def jump(self, label):
        self.stream.append({ 'op' : 'jump', 'label' : label })

    def iemit(self, varname):
        self.stream.append({ 'op' : 'iemit', 'var' : varname })

    def semit(self, varname):
        self.stream.append({ 'op' : 'semit', 'var' : varname })

    def sleep(self):
        pass

    def label(self, name):
        self.stream.append({ 'op' : 'labeldef', 'name' : name })
        self.labels[name] = 0;

    def pausems(self, value):
        self.stream.append({ 'op' : 'pausems', 'value' : value})

    def pauses(self, value):
        self.stream.append({ 'op' : 'pauses', 'value' : value})

    def emit_semit(self, param, object):
        if self.sconsts.get(param['var'], None):
            object.append(ord('e'))
            object.append(self.sconsts[param['var']]['alias'])
        else:
            object.append(ord('E'))
            object.append(self.svars[param['var']]['alias'])
        return object

    def emit_svar(self, param, object):
        value = self.svars[param['var']]['value']
        object.append(ord('s'))
        object.append(self.svars[param['var']]['alias'])
        object.append(len(value) + 1)
        for xx in value:
            object.append(ord(xx))
        return object

    def emit_pausems(self, param, object):
        value = int(param['value'])
        object.append(ord('p'))
        object.append((value >> 8) & 0xff)
        object.append(value & 0xff)
        return object

    def emit_pauses(self, param, object):
        value = int(param['value'])
        object.append(ord('P'))
        object.append((value >> 8) & 0xff)
        object.append(value & 0xff)
        return object

    def emit_labeldef(self, param, object):
        return object

    def emit_jump(self, param, object):
        object.append(ord('j'))
        jumpTo = int(self.labels[param['label']])
        object.append((jumpTo >> 8) & 0xff)
        object.append(jumpTo & 0xff)
        return object

    def emit_dec(self, param):
        pass

    def emit_inc(self, param):
        pass

    def emit_iemit(self, param):
        pass

    emitMap = {
        'semit' : 'emit_semit',
        'iemit' : 'emit_iemit',
        'svar' : 'emit_svar',
        'pausems' : 'emit_pausems',
        'pauses' : 'emit_pauses',
        'labeldef' : 'emit_labeldef',
        'jump' : 'emit_jump',
        'dec' : 'emit_dec',
        'inc' : 'emit_inc'
    }

    def finalize(self, sconsts, svars, ivars):
        [self.object.append(xx) for xx in (0, 0)] # make room for code start offset

        # map sconsts to single byte aliases
        aliasId = 0
        self.sconsts = self._buildVarAliases(sconsts, aliasId)
        for name, const in sconsts.iteritems():
            self.object.append(const['alias'])
            self.object.append(len(const['value']) + 1)
            [self.object.append(ord(xx)) for xx in const['value']]

        aliasId += len(sconsts)
        self.svars = self._buildVarAliases(svars, aliasId)

        # map ivars to single byte aliases
        aliasId += len(svars)
        self.ivars = self._buildVarAliases(ivars, aliasId)
        self.ivars = self._mapIVarLocations(ivars)
            
        # set instruction offset into object
        (address, length) = self.object.buffer_info()
        self.object[0] = (length >> 8) & 0xff
        self.object[1] = length & 0xff

        # fix up label locations
        throwaway = array.array('B')
        offset = length 

        for cmd in self.stream:
            offset += throwaway.buffer_info()[1]
            throwaway = getattr(self, ByteStream.emitMap[cmd['op']])(cmd, throwaway)
            if 'labeldef' == cmd['op']:
                self.labels[cmd['name']] = offset


        for cmd in self.stream:
            getattr(self, ByteStream.emitMap[cmd['op']])(cmd, self.object)

        self.object.append(ord('!'))
        self.object.tofile(open('obj.bin', 'w'))

# map svars to RAM locations
# map ivars to RAM locations
        pass
