#!/usr/bin/env python3

import pdb
from picam_utils import *
import time

#global picam
#camera = {'picam': None}

#pdb.set_trace()
logger = setLogging('find_sprocket_test','find_sprocket_test.log',logging.DEBUG)['logger']
camera = init_picam(exposure=1000)['camera']

logger.debug('Sleep 10')

time.sleep(10.0)

for ii in range(10):
    inccount()
    findSprocket(logger, camera.capture_array("lores"), savework=True)
    logger.info(camera.capture_metadata()['ExposureTime'])
    logger.info('sleep 4')
    time.sleep(4.0)
