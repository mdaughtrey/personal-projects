#!/usr/bin/python

import sys
import pdb
import re
import ByteStream
import array
import re

verbose = False

labels = {}

def vOut(text):
    if verbose:
        sys.stdout.write(text)

class Parser:
    def __init__(self, lines):
        self.sconsts = {}
        self.iconsts = {}
#        self.svars = {}
#        self.ivars = {}
        self.streamer = ByteStream.ByteStream()
        for line in lines:
            line = line.strip()

            if 0 == len(line):
                continue

            try:
                for (regex, command) in Parser.commandMap.items():
                    if re.search(regex, line):
                        getattr(self, command)(line)
                        raise StopIteration

                sys.stdout.write("Error processing [%s]\n" % line)
                sys.exit(1)

            except StopIteration:
                pass

    def unescapeString(self, string):
        values = string.split('\\x')
        result = array.array('b')

        if len(values) > 1:
            result.fromstring(values[0])
            for value in values[1:]:
                ii = int(value[0:2], 16)
                result.append(ii)
                result.fromstring(value[2:])
        else:
            result.fromstring(string)

        return result

    def finalize(self):
        self.streamer.finalize(self.sconsts, self.iconsts) #, self.svars) # , self.ivars)

    def cmd_comment(self, param):
        pass

    def cmd_sconst(self, param):
        name = param.split()[1]
        value = ' '.join(param.split()[2:])

        if re.match('"', value):
            value = re.sub(r'\"(.*)\"$', r'\1', value)
            value = self.unescapeString(value).tostring()

        if re.match("'", value):
            value = re.sub(r"\'(.*)\'$", r'\1', value)
            value = self.unescapeString(value).tostring()

        self.sconsts[name] = { 'value' : value }
        vOut ("Setting SCONST %s to [%s]\n" % (name, self.sconsts[name]))

    def cmd_iconst(self, param):
        name = param.split()[1]
        value = int(param.split()[2])
        self.iconsts[name] = { 'value' : value }
        vOut ("Setting ICONST %s to [%d]\n" % (name, self.iconsts[name]))

    def cmd_svar(self, param):
        name = param.split()[1]
        value = ' '.join(param.split()[2:])
        value = self.unescapeString(value.replace('"', '')).tostring()
#        self.svars[name] = { 'value' : value , 'length' : len(value) }
        vOut ("Setting SVAR %s to [%s]\n" % (name, value))
#        self.streamer.svar(name, self.svars[name]['value'])
        self.streamer.svar(name, value)

    def cmd_ivar(self, param):
        name = param.split()[1]
        assign = param.split()[2]
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

        vOut ("Setting IVAR %s to [%s]\n" % (name, assign))
        self.streamer.ivar(name, assign)

    def cmd_dec(self, param):
        name = param.split()[1]
        self.streamer.dec(name)

    def cmd_inc(self, param):
        name = param.split()[1]
        self.streamer.inc(name)

    def cmd_jz(self, param):
        varname = param.split()[1]
        label = param.split()[2]
        self.streamer.jz(varname, label)

    def cmd_jump(self, param):
        labelname = param.split()[1]
        self.streamer.jump(labelname)

    def cmd_iemit(self, param):
        (op, name) = param.split(' ', 2)
        self.streamer.iemit(name)

    def cmd_semit(self, param):
        (op, name) = param.split(' ', 2)
        self.streamer.semit(name)

    def cmd_label(self, param):
        (name) = param.split(':', 1)[0]
        self.streamer.label(name)

    def cmd_pausems(self, param):
        value = param.split()[1]
        self.streamer.pausems(int(value))

    def cmd_pauses(self, param):
        value = param.split()[1]
        self.streamer.pauses(int(value))

    def cmd_add(self, param):
        var1 = param.split()[1]
        var2 = param.split()[2]
        self.streamer.add(var1, var2)

    def cmd_sub(self, param):
        var1 = param.split()[1]
        var2 = param.split()[2]
        self.streamer.sub(var1, var2)

    def cmd_end(self, param):
        self.streamer.end()    


    commandMap = {
        '^#' : 'cmd_comment',
        '^add\s'    : 'cmd_add',
        '^dec\s'    : 'cmd_dec',
        '^iemit\s'  : 'cmd_iemit',
        '^inc\s'    : 'cmd_inc',
        '^ivar\s'   : 'cmd_ivar',
        '^jump\s'   : 'cmd_jump',
        '^jz\s'     : 'cmd_jz',
        '^mod\s'    : 'cmd_mod',
        '^pausems'  : 'cmd_pausems',
        '^pauses'   : 'cmd_pauses',
        '^sconst\s' : 'cmd_sconst',
        '^iconst\s' : 'cmd_iconst',
        '^semit\s'  : 'cmd_semit',
        '^sub\s'    : 'cmd_sub',
        '^svar\s'   : 'cmd_svar',
        '^\S+:'     : 'cmd_label',
        'end'       : 'cmd_end'
    }


verbose = ('-v' in sys.argv)

parser = Parser(open('clock.pk').readlines())
#parser = Parser(open('sconst.pk').readlines())
parser.finalize()

