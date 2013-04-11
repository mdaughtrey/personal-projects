import array
import pdb
import re

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
        sVar = { 'op' : 'svar', 'var' : name, 'value' : value,
            'length' : len(value) + 1, 'location' : -1 }
        self.svars[name]  = sVar
        self.stream.append(sVar)

    def ivar(self, name, assign):
        value = {}
        if re.match('0x', assign):
            value = { 'op' : 'ivarvalue', 'value' : int(assign, 16), 'length' : 2 }
        else:
            try:
                value = { 'op' : 'ivarvalue', 'value' : int(assign), 'length' : 2 }

            except:
                value = { 'op' : 'ivarivar', 'assignfrom' : assign, 'length' : 2 }

        iVar = { 'name' : name, 'location' : -1 }
        for k, v in value.iteritems():
            iVar[k] = v

        self.ivars[name] = iVar
        self.stream.append(iVar)

#        if re.match('0x', text):
#            value = int(text, 16)
#        else:
#            value = int(text)
#
#        try:
#            float(value)
#
#        except TypeError:
#            sys.stdout.write("Value [%s] is not a number\n" % value)
#            sys.exit(1)

    def end(self):
        self.stream.append({ 'op' : 'end' })

    def dec(self, name):
        self.stream.append({ 'op' : 'dec', 'value' : 0, 'var' : name })

    def inc(self, name):
        self.stream.append({ 'op' : 'inc', 'value' : 0, 'var' : name })

    def jz(self, varname, label):
        self.stream.append({ 'op' : 'jz', 'var' : varname, 'label' : label })

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

    def add(self, var1, var2):
        self.stream.append({ 'op' : 'add', 'var1' : var2, 'var2' : var2 })

    def sub(self, var1, var2):
        self.stream.append({ 'op' : 'sub', 'var1' : var1, 'var2' : var2 })

    def emit_semit(self, param):
        emitted = array.array('B')
        if self.sconsts.get(param['var'], None):
            emitted.append(ord('e'))
            emitted.append(self.sconsts[param['var']]['alias'])
        else:
            emitted.append(ord('S'))
            emitted.append((self.svars[param['var']]['location'] >> 8) & 0xff)
            emitted.append( self.svars[param['var']]['location'] & 0xff)
        return emitted

    def emit_svar(self, param):
        emitted = array.array('B')
        emitted.append(ord('s'))
        emitted.append((param['location'] >> 8) & 0xff)
        emitted.append(param['location'] & 0xff)
        emitted.append(len(param['value']) + 1)
        for xx in param['value']:
            emitted.append(ord(xx))
        return emitted

    def emit_pausems(self, param):
        emitted = array.array('B')
        value = int(param['value'])
        emitted.append(ord('p'))
        emitted.append((value >> 8) & 0xff)
        emitted.append(value & 0xff)
        return emitted

    def emit_pauses(self, param):
        emitted = array.array('B')
        value = int(param['value'])
        emitted.append(ord('P'))
        emitted.append((value >> 8) & 0xff)
        emitted.append(value & 0xff)
        return emitted

    def emit_labeldef(self, param):
        emitted = array.array('B')
        return emitted

    def emit_jump(self, param):
        emitted = array.array('B')
        emitted.append(ord('j'))
        jumpTo = int(self.labels[param['label']])
        emitted.append((jumpTo >> 8) & 0xff)
        emitted.append(jumpTo & 0xff)
        return emitted

    def emit_ivarivar(self, param):
        emitted = array.array('B')
        emitted.append(ord('m'))
        emitted.append((self.ivars[param['name']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['name']]['location']  & 0xff)
        emitted.append((self.ivars[param['assignfrom']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['assignfrom']]['location']  & 0xff)
        return emitted

    def emit_ivarvalue(self, param):
        emitted = array.array('B')
        emitted.append(ord('i'))

        location = self.ivars[param['name']]['location']

        emitted.append((location >> 8) & 0xff)
        emitted.append(location & 0xff)
        emitted.append((param['value'] >> 8) & 0xff)
        emitted.append(param['value'] & 0xff)
        return emitted

    def emit_dec(self, param):
        emitted = array.array('B')
        emitted.append(ord('-'))
        location = self.ivars[param['var']]['location']
        emitted.append((location >> 8) & 0xff)
        emitted.append(location & 0xff)
        return emitted

    def emit_inc(self, param):
        emitted = array.array('B')
        emitted.append(ord('+'))
        location = self.ivars[param['var']]['location']
        emitted.append((location >> 8) & 0xff)
        emitted.append(location & 0xff)
        return emitted

    def emit_iemit(self, param):
        emitted = array.array('B')
        emitted.append(ord('I'))
        emitted.append((self.ivars[param['var']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var']]['location'] & 0xff)
        return emitted

    def emit_jz(self, param):
        emitted = array.array('B')
        emitted.append(ord('z'))
        emitted.append((self.ivars[param['var']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var']]['location'] & 0xff)
        emitted.append((self.labels[param['label']] >> 8) & 0xff)
        emitted.append( self.labels[param['label']] & 0xff)
        return emitted

    def emit_add(self, param):
        emitted = array.array('B')
        emitted.append(ord('a'))
        emitted.append((self.ivars[param['var1']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var1']]['location'] & 0xff)
        emitted.append((self.ivars[param['var2']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var2']]['location'] & 0xff)
        return emitted

    def emit_sub(self, param):
        emitted = array.array('B')
        emitted.append(ord('u'))
        emitted.append((self.ivars[param['var1']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var1']]['location'] & 0xff)
        emitted.append((self.ivars[param['var2']]['location'] >> 8) & 0xff)
        emitted.append( self.ivars[param['var2']]['location'] & 0xff)
        return emitted

    def emit_end(self, param):
        emitted = array.array('B')
        emitted.append(ord('!'))
        return emitted

    emitMap = {
        'add' : 'emit_add',
        'dec' : 'emit_dec',
        'iemit' : 'emit_iemit',
        'inc' : 'emit_inc',
        'ivarvalue' : 'emit_ivarvalue',
        'ivarivar' : 'emit_ivarivar', 
        'jump' : 'emit_jump',
        'jz'   : 'emit_jz',
        'labeldef' : 'emit_labeldef',
        'pauses' : 'emit_pauses',
        'pausems' : 'emit_pausems',
        'semit' : 'emit_semit',
        'sub' : 'emit_sub',
        'svar' : 'emit_svar',
        'end'  : 'emit_end'
    }

    def finalize(self, sconsts, iconsts): #, svars): # , ivars):
        [self.object.append(xx) for xx in (0, 0)] # make room for code start offset

        # map sconsts to single byte aliases
        aliasId = 0
        self.sconsts = self._buildVarAliases(sconsts, aliasId)
        for name, const in sconsts.iteritems():
            self.object.append(const['alias'])
            self.object.append(len(const['value']) + 1)
            [self.object.append(ord(xx)) for xx in const['value']]

        # map iconsts to single byte aliases
        self.iconsts = self._buildVarAliases(iconsts, aliasId)
        for name, const in iconsts.iteritems():
            self.object.append(const['alias'])
            self.object.append(2)
            self.object.append((const['value'] >> 8) & 0xff)
            self.object.append(const['value'] & 0xff)

        if self.svars:
            self.svars = self._buildVarLocations(self.svars)
        if self.ivars:
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
            throwaway = getattr(self, ByteStream.emitMap[cmd['op']])(cmd)
            if 'labeldef' == cmd['op']:
                print "resolving label %s to location %d" % (cmd['name'], offset)
                self.labels[cmd['name']] = offset

        for cmd in self.stream:
            self.object.extend(getattr(self, ByteStream.emitMap[cmd['op']])(cmd))

        self.object.append(ord('!'))
        self.object.tofile(open('obj.bin', 'w'))
