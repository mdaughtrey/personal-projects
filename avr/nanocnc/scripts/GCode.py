#!/usr/bin/python

import pdb
import re
import sys
import math
import serial
import re

class MoveSpeed():
    fast = 0
    slow = 1

class MoveType():
    linear = 0
    arcCW = 1
    arcCCW = 2

class CoordMode():
    relative = 0
    absolute = 1

class Units():
    mm = 0
    inches = 1

class GCode():
    def __init__(self):
        self.myCodes = ''
        self.xPos = 0.0
        self.yPos = 0.0
        self.zPos = 0.0
        self.x = None
        self.y = None
        self.z = None
        self.i = None
        self.j = None
        self.k = None
        self.r = None
        self.coordMode = CoordMode.absolute
        self.moveSpeed = MoveSpeed.fast
        self.moveType = MoveType.linear
        self.units = Units.inches
        self.dev = serial.Serial('/dev/tty.usbserial-A4001g2G', 9600, timeout=0.01) 
        self.dev.write("i\r");
        self.dev.write("zc");

    def __emitCommand(self, line):
        self.myCodes += line
        print "[%s]" % line
        return
        line = line.rstrip('\r\n') + '\r'
        self.dev.write(line)
        proceed = False
        while False == proceed:
            rx = ''.join(self.dev.readlines())
            if re.search('OK\r\n', rx):
                proceed = True

    def __getQuadrant(self, x, y):
        if x >= self.centerX and y >= self.centerY:
            return 0
        elif x >= self.centerX and y < self.centerY:
            return 1                
        elif x < self.centerX and y < self.centerY:
            return 2
        elif x < self.centerX and y >= self.centerY:
            return 3
        else:
            print "Invalid Quadrant Calculation"
            sys.exit(1)

    def __fRound(self, test):
        if ((int(test) - 0.001) < test < (int(test) + 0.001)):
            return int(test)
        return test

    def __buildArcCW(self):
        pdb.set_trace()
        if self.r:
            pass
        else:
            self.centerX = self.xPos + self.i
            self.centerY = self.yPos + self.j
            startQ = self.__getQuadrant(self.xPos, self.yPos)
            endQ = self.__getQuadrant(self.x, self.y)
            length = math.sqrt(abs((self.xPos - self.centerX)) ** 2 + abs(self.yPos - self.centerY) ** 2)

            try:
                deg = math.degrees(math.asin((self.xPos - self.centerX) / length)) #+ (90 * startQ)
                degEnd = math.degrees(math.acos((self.y- self.centerY) / length)) #+ (90 * endQ)
            except ValueError as vv:
                pass
            while deg <= degEnd:
                try:
                    pointX = self.centerX + math.sin(math.radians(deg)) * length
                    pointY = self.centerY + math.cos(math.radians(deg)) * length
                except ValueError as vv:
                    pdb.set_trace()
                command = "goto %d %d %d\r\n" % (round(pointX, 0), round(pointY, 0), round(self.zPos, 0))
                self.__emitCommand(command)
                deg += 1.0
            self.xPos = self.__fRound(pointX)
            self.yPos = self.__fRound(pointY)

    def __buildArcCCW(self):
        pass

    def buildCommand(self):
#        pdb.set_trace()
        if MoveType.arcCW == self.moveType:
            self.__buildArcCW()
            return
        if MoveType.arcCCW == self.moveType:
            self.__buildArcCCW()
            return

        if CoordMode.relative == self.coordMode:
            if self.x is not None:
                self.xPos += self.x
            if self.y is not None:
                self.yPos += self.y
            if self.z is not None:
                self.zPos += self.z
        else:
            if self.x is not None:
                self.xPos = self.x
            if self.y is not None:
                self.yPos = self.y
            if self.z is not None:
                self.zPos = self.z

        command = "goto %d %d %d\r\n" % (round(self.xPos, 0), round(self.yPos, 0), round(self.zPos, 0))
        self.x = None
        self.y = None
        self.z = None
        self.__emitCommand(command)

    def __parseInt(self, line, lookFor):
        keyString = "[" + lookFor.upper() + lookFor.lower() + ']\s*(-?\d+)'
        found = re.search(keyString, line)
        if found:
            setattr(self, lookFor.lower(), int(found.group(1)))
        else:
            setattr(self, lookFor.lower(), None)
        line = re.sub(keyString, '', line)
        return line

    def __parseG(self, line):
        keyString = 'G\s*(\d+)'
        found = re.search(keyString, line)
        if found:
            gMode = int(found.group(1))
            if 0 == gMode:
                self.moveType = MoveType.linear
                self.moveSpeed = MoveSpeed.fast
            elif 1 == gMode:
                self.moveType = MoveType.linear
                self.moveSpeed = MoveSpeed.slow
            elif 2 == gMode:
                self.moveType = MoveType.arcCW
                self.moveSpeed = MoveSpeed.slow
            elif 3 == gMode:
                self.moveType = MoveType.arcCCW
                self.moveSpeed = MoveSpeed.slow
            elif 71 == gMode:
                self.units = Units.mm
            elif 90 == gMode:
                self.coordMode = CoordMode.absolute

        line = re.sub(keyString, '', line)
        return line


    def intParameter(self, line):
        for testFor in ['Xx', 'Yy', 'Zz', 'Ii', 'Jj', 'Rr' ]: 
            if line[0] in testFor:
                return (True, self.__parseInt(line, testFor[0]))
        return (False, line)

    def parse(self, line):
        print "Parsing %s" % line
        line = line.rstrip()
#        pdb.set_trace()
        while len(line):
            line = line.strip()
            if line[0] in 'Gg':
                line = self.__parseG(line)
                continue

            if line[0] == '(':
                line = ''
                return True

            (result, line) = self.intParameter(line)
            if False == result:
                print "Unknown command [%s]" % line
                return False

        self.buildCommand()
        return True

    def commands(self):
        return self.myCodes


gCode = GCode()

for line in open('gcode.gc', 'r'):
    if False == gCode.parse(line):
        sys.exit(1)

open('test.cmd', 'w').write(gCode.commands())

