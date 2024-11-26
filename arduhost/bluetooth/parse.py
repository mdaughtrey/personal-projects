#!/bin/env python3

import pdb
import re

msg='WIFI:S:yfinity;T:WPA;P:24HoursADay;H:false;;'
pdb.set_trace()
data = re.match('WIFI:S:(\w+);T:(\w+);P:(\w+);', msg)
pass

