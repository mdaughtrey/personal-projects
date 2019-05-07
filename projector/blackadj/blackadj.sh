#!/bin/bash

RAWSRC=/media/sf_vproj/scans/test/001/rawfile/000000a.RAW
RAWDEST=./test.RAW
PPM=./test.ppm
DCRAW=/personal-projects/projector/raspiraw/dcraw/dcraw

proc ()
{
    cp $1 $RAWDEST
    $DCRAW $RAWDEST
    python3 blackadj.py
    cp test.png /media/sf_vproj/scans/$(basename ${1/RAW/png})
}


for raw in /media/sf_vproj/scans/test/001/rawfile/000000a.RAW \
    /media/sf_vproj/scans/test/001/rawfile/000000b.RAW \
    /media/sf_vproj/scans/test/001/rawfile/000000c.RAW; do
    proc $raw
done
