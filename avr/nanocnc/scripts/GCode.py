#!/usr/bin/python

import pdb
import re

class GCode():
    def __init__(self):
        self.xPos = 0
        self.yPos = 0
        self.zPos = 0
        self.myCodes = ''
        self.xDelta = 0
        self.yDelta = 0
        self.zDelta = 0

    def buildCommand(self):
        command = "GOTO %s %s %s\r" % (self.xDelta, self.yDelta, self.zDelta)
        self.xDelta = 0
        self.yDelta = 0
        self.zDelta = 0
        self.myCodes += command

    def __parseX(self, line):
        keyString = 'X\s*(\d+)'
        found = re.search(keyString, line)
        if found:
            self.xDelta = found.group(1)
        line = re.sub(keyString, '', line)
        return line

    def __parseY(self, line):
        keyString = 'Y\s*(\d+)'
        found = re.search(keyString, line)
        if found:
            self.yDelta = found.group(1)
        line = re.sub(keyString, '', line)
        return line

    def __parseZ(self, line):
        pdb.set_trace()
        keyString = 'Z\s*(\d+)'
        found = re.search(keyString, line)
        if found:
            self.zDelta = found.group(1)
        line = re.sub(keyString, '', line)
        return line

    def __parseG(self, line):
        keyString = 'G\s*(\d+)'
        line = re.sub(keyString, '', line)
        return line

    def parse(self, line):
        print "parse: %s" % line
        line = line.rstrip()
        while len(line):
            line = line.strip()
            print "Parsing %s" % line
            if line[0] in 'Xx':
                line = self.__parseX(line)
                continue
            if line[0] in 'Yy':
                line = self.__parseY(line)
                continue
            if line[0] in 'Zz':
                line = self.__parseZ(line)
                continue
            if line[0] in 'Gg':
                line = self.__parseG(line)
                continue
        self.buildCommand()

    def commands(self):
        return self.myCodes


gCode = GCode()

for line in open('gcode.gc', 'r'):
    gCode.parse(line)

print gCode.commands()

