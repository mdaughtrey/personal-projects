#!/bin/bash

NUMFRAMES=100

./runproj.py --targetdir ./tmp --numframes $NUMFRAMES --pretension 20 --mode 8mm 
rc=$?

echo rc $rc

# /Volumes/imageinput/Niall/NK0032
