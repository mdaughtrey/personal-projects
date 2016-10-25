#!/usr/bin/python

import argparse
import httplib

argparser = argparse.ArgumentParser()
argparser.add_argument('--test', dest = 'test')

args = argparser.parse_args()


def init():
    conn = httplib.HTTPConnection('192.168.107.1/mode/control')
    conn 

def main():
    pass

main()
