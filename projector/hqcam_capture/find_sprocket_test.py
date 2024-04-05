#!/usr/bin/env python3

import pdb
from picam_utils import *
import time

#global picam
#camera = {'picam': None}

pdb.set_trace()
logger = setLogging('find_sprocket_test','find_sprocket_test.log')
camera = init_picam(exposure=9000)
time.sleep(1.0)

for ii in range(10):
    inccount()
    findSprocket(logger['logger'], camera['picam'].capture_array("lores"), savework=True)
    time.sleep(1.0)
