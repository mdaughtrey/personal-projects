import array
import pdb

class ByteStream:
    def __init__(self):
        self.labels = {} 
        self.ivars = {}
        self.svars = {}
        self.stream = []
        self.object = array.array('B')
        self.ramLocation = 0

    def _buildVarAliases(self, varMap, aliasBase):
        for key, value in varMap.iteritems():
            varMap[key]['alias'] = aliasBase
            aliasBase += 1
        return varMap

    def _buildVarLocations(self, varMap):
        for key, value in varMap.iteritems():
            varMap[key]['location'] = self.ramLocation
            self.ramLocation += varMap[key]['length']
        return varMap

    def svar(self, name, value):
        sVar = { 'op' : 'svar', 'var' : name, 'value' : value, 'length' : len(value) + 1, 'location' : -1 }
        self.svars[name]  = sVar
        self.stream.append(sVar)

    def ivar(self, name, value):
        iVar = { 'op' : 'ivar', 'name' : name, 'value' : value, 'length' : 2, 'location' : -1 }
        self.ivars[name] = iVar
        self.stream.append(iVar)

    def dec(self, name):
        self.stream.append({ 'op' : 'dec', 'value' : 0, 'var' : name })

    def inc(self, name):
        self.stream.append({ 'op' : 'inc', 'value' : 0, 'var' : name })

    def jz(self, name, label):
        self.stream.append({ 'op' : 'jz', 'label' : label })

    def jump(self, label):
        self.stream.append({ 'op' : 'jump', 'label' : label })

    def iemit(self, varname):
        self.stream.append({ 'op' : 'iemit', 'var' : varname })

    def semit(self, varname):
        self.stream.append({ 'op' : 'semit', 'var' : varname })

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
            object.append(ord('S'))
            object.append((self.svars[param['var']]['location'] >> 8) & 0xff)
            object.append( self.svars[param['var']]['location'] & 0xff)
        return object

    def emit_svar(self, param, object):
        object.append(ord('s'))
        object.append((param['location'] >> 8) & 0xff)
        object.append(param['location'] & 0xff)
        object.append(len(param['value']) + 1)
        for xx in param['value']:
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

    def emit_ivar(self, param, object):
        object.append(ord('i'))
        object.append((param['location'] >> 8) & 0xff)
        object.append(param['location'] & 0xff)
        object.append((param['value'] >> 8) & 0xff)
        object.append(param['value'] & 0xff)
        return object

    def emit_dec(self, param, object):
        object.append(ord('-'))
        location = self.ivars[param['var']]['location']
        object.append((location >> 8) & 0xff)
        object.append(location & 0xff)
        return object

    def emit_inc(self, param, object):
        object.append(ord('+'))
        location = self.ivars[param['var']]['location']
        object.append((location >> 8) & 0xff)
        object.append(location & 0xff)
        return object

    def emit_iemit(self, param, object):
        object.append(ord('I'))
        object.append((self.ivars[param['var']]['location'] >> 8) & 0xff)
        object.append( self.ivars[param['var']]['location'] & 0xff)
        return object

    emitMap = {
        'semit' : 'emit_semit',
        'iemit' : 'emit_iemit',
        'svar' : 'emit_svar',
        'ivar' : 'emit_ivar',
        'pausems' : 'emit_pausems',
        'pauses' : 'emit_pauses',
        'labeldef' : 'emit_labeldef',
        'jump' : 'emit_jump',
        'dec' : 'emit_dec',
        'inc' : 'emit_inc'
    }

    def finalize(self, sconsts): #, svars): # , ivars):
        [self.object.append(xx) for xx in (0, 0)] # make room for code start offset

        # map sconsts to single byte aliases
        aliasId = 0
        self.sconsts = self._buildVarAliases(sconsts, aliasId)
        for name, const in sconsts.iteritems():
            self.object.append(const['alias'])
            self.object.append(len(const['value']) + 1)
            [self.object.append(ord(xx)) for xx in const['value']]

        self.svars = self._buildVarLocations(self.svars)
        self.ivars = self._buildVarLocations(self.ivars)
            
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
            if 'semit' == cmd['op']:
                cmd['location'] = self.svars[cmd['var']]['location']
            if 'iemit' == cmd['op']:
                cmd['location'] = self.ivars[cmd['var']]['location']


        for cmd in self.stream:
            pdb.set_trace()
            getattr(self, ByteStream.emitMap[cmd['op']])(cmd, self.object)

        self.object.append(ord('!'))
        self.object.tofile(open('obj.bin', 'w'))
