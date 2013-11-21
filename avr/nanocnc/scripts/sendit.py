#!/usr/bin/python

import sys
import serial
import time
import re
import pdb

CIRCLE_DIA = 10 # pixels

def openDevice():
    dev = serial.Serial('/dev/tty.usbserial-A4001g2G', 9600, timeout=0.1) 
    return dev

def expect(string):
    pass


def main():
    try:
# pdb.set_trace()
        dev = openDevice()
        dev.write("i\r");
        dev.write("zc");
        print dev.readlines()

        ff = open('test.cmd', 'r')
        lines = ff.readlines()
        for line in lines:
            line = line.rstrip('\r\n') + '\r'
            print "[%s]" % line
            dev.write(line)
            proceed = False
            while False == proceed:
                rx = ''.join(dev.readlines())
                if re.search('OK\r\n', rx):
                    proceed = True


    except:
        print "could not open file"
        sys.exit(0)

main()
