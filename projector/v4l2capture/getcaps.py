#!/usr/bin/env python3

import  cv2
import pdb

cap = cv2.VideoCapture('/dev/video0')

props = [(a,cap.get(getattr(cv2, a))) for a in dir(cv2) if a.startswith('CAP_PROP')]
for k,v in props:
    if -1.0 == v: continue
    print(f'{k}: {v}')
