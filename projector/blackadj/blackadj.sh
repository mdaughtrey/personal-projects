#!/bin/bash

RAWSRC=/media/sf_vproj/scans/test/001/rawfile/000000c.RAW
RAWDEST=./test.RAW
PPM=./test.ppm
DCRAW=/home/mattd/personal-projects/projector/dcraw/dcraw

if [ ! -f "${RAWDEST}" ]; then cp $RAWSRC $RAWDEST; fi
if [ ! -f "${PPM}" ]; then $DCRAW $PPM

#python blackadj.py
