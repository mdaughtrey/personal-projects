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
        self.svars = {}
        self.ivars = {}
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
        self.streamer.finalize(self.sconsts, self.svars, self.ivars)

    def cmd_comment(self, param):
        pass

    def cmd_sconst(self, param):
        name = param.split()[1]
        value = ' '.join(param.split()[2:])
        self.sconsts[name] = self.unescapeString(value.replace('"', '')).tostring()
        vOut ("Setting SCONST %s to [%s]\n" % (name, self.sconsts[name]))

    def cmd_svar(self, param):
        name = param.split()[1]
        value = ' '.join(param.split()[2:])
        self.svars[name] = self.unescapeString(value.replace('"', '')).tostring()
        vOut ("Setting SVAR %s to [%s]\n" % (name, self.svars[name]))
        self.streamer.svar(name, self.svars[name])

    def cmd_ivar(self, param):
        (op, name, value) = param.split(' ', 2)

        try:
            float(value)

        except TypeError:
            sys.stdout.write("Value [%s] is not a number\n" % value)
            sys.exit(1)

        self.ivars[name] = value
        vOut ("Setting IVAR %s to [%s]\n" % (name, value))

    def cmd_dec(self, param):
    # does ivar exist?
        pass

    def cmd_inc(self, param):
    # does ivar exist?
        pass

    def cmd_jz(self, param):
    # does ivar exist?
    # does label exist?
        pass

    def cmd_jump(self, param):
        labelname = param.split()[1]
        self.streamer.jump(labelname)

    def cmd_iemit(self, param):
    # does var exist?
        pass

    def cmd_semit(self, param):
        (op, name) = param.split(' ', 2)
        self.streamer.semit(name)

    def cmd_sleep(self, param):
        pass

    def cmd_label(self, param):
        (name) = param.split(':', 1)[0]
        self.streamer.label(name)

    def cmd_pausems(self, param):
        value = param.split()[1]
        self.streamer.pausems(int(value))

    def cmd_pauses(self, param):
        value = param.split()[1]
        self.streamer.pauses(int(value))

    commandMap = {
        '^#' : 'cmd_comment',
        '^sconst\s' : 'cmd_sconst',
        '^svar\s'   : 'cmd_svar',
        '^ivar\s'   : 'cmd_ivar',
        '^dec\s'    : 'cmd_dec',
        '^inc\s'    : 'cmd_inc',
        '^jz\s'     : 'cmd_jz',
        '^jump\s'   : 'cmd_jump',
        '^iemit\s'  : 'cmd_iemit',
        '^semit\s'  : 'cmd_semit',
        '^sleep\s'  : 'cmd_sleep',
        '^\S+:'     : 'cmd_label',
        '^pausems'   : 'cmd_pausems',
        '^pauses'    : 'cmd_pauses'
    }


verbose = ('-v' in sys.argv)

parser = Parser(open('semit.pk').readlines())
parser.finalize()

