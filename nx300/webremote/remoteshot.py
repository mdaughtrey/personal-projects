#!/usr/bin/python

import httplib
import time

conn = httplib.HTTPConnection('192.168.0.33', 80)
for cmd in ['down=Super_L','down=Super_R','up=Super_L','up=Super_R']:
    url = '/api/v1/input/inject?key%s' % cmd
    print url
    conn.request('GET', url)
    response = conn.getresponse()
    #print('Status %s' % response.status)
    #print('Reason %s' % response.reason)
    #print('Headers %s' % response.getheaders())
    data = response.read()
    #print('Data %s' % data)
