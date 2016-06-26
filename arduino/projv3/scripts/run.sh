#!/bin/bash

NUMFRAMES=100

exec ./runproj.py --targetdir ./tmp --numframes $NUMFRAMES --pretension 20 --mode 8mm 
#exec ./runproj.py --targetdir ./tmp --transferonly
