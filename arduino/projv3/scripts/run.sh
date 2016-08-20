#!/bin/bash

NUMFRAMES=20
MNTPOINT=/Volumes/imageinput
SERVER=smb://GUEST:@nas-36-FE-22%20%28SMB%29._smb._tcp.local/imageinput
#TARGETDIR=/Volumes/imageinput/tmp
#TARGETDIR=./scans/Niall/NK0032
TARGETDIR=./scans/test/001
MODE=8mm
WIFIDEV=en0

#let retry=3
#while [[ ! -d $MNTPOINT ]]
#do
#    open $SERVER
#    sleep 10
#    ((retry--))
#    if ((0 == retry))
#    then
#        echo Cannot mount, giving up
#    fi
#done

if [[ ! -d "$TARGETDIR" ]]
then
    mkdir -p $TARGETDIR
fi

arg=${1:-capture}

if [[ "capture" == "$arg" ]]
then
    exec ./runproj.py --targetdir $TARGETDIR --numframes $NUMFRAMES --pretension 10 --mode $MODE --wifidev $WIFIDEV
else
    exec ./runproj.py --targetdir $TARGETDIR --transferonly --wifidev $WIFIDEV
fi
