#!/bin/bash

RAWSRC=/media/sf_vproj/scans/test/001/rawfile/000000c.RAW
RAWDEST=./test.RAW
PPM=./test.ppm
DCRAW=/personal-projects/projector/raspiraw/dcraw/dcraw

if [ ! -f "${RAWDEST}" ]; then cp $RAWSRC $RAWDEST; fi
if [ ! -f "${PPM}" ]; then $DCRAW $RAWDEST; fi
if [ -f "${PPM}" ]; then python3 blackadj.py; fi
