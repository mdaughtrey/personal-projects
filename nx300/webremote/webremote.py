#!/usr/bin/python

import argparse
import httplib

argparser = argparse.ArgumentParser()
argparser.add_argument('--test', dest = 'test')

args = argparser.parse_args()


def init():
    conn = httplib.HTTPConnection('192.168.107.1', 7788)
    headers = {
        'User-Agent': 'SEC_MODE_+1231231234',
        'Connection': 'Close',
        'NTS': 'Alive',
        'HOST-Mac': '02:00:00:00:00:00',
        'HOST-Address': '192.168.0.18',
        'HOST-port': '7788',
        'HOST-PNumber': '+1231231234',
        'Access-Method': 'Manual',
        'CALLBACK': '<http://192.168.0.18:7792/eventCallback>'
    }
    conn.request('HEAD', '/mode/control', None, headers)
    response = conn.getresponse()
    print response.status, response.reason

    data = response.read()
    print data
    conn.close()

def main():
    init()

main()
