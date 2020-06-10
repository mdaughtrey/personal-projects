#!/bin/bash

NUMFRAMES=200
MNTPOINT=/Volumes/imageinput
SERVER=smb://GUEST:@nas-36-FE-22%20%28SMB%29._smb._tcp.local/imageinput
#TARGETDIR=/Volumes/imageinput/tmp
#TARGETDIR=./scans/Niall/NK0032
#TARGETDIR=./scans/test/001
TARGETDIR=~/Documents/vmshared/scans/Niall/NK0032
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
if [[ "$1" != "" ]]
then
    ./runproj.py --targetdir $TARGETDIR --transferonly --wifidev $WIFIDEV
    exit 0
fi

while [[ 0 ]]
do
    ./runproj.py --targetdir $TARGETDIR --numframes $NUMFRAMES --mode $MODE --wifidev $WIFIDEV
    echo scanresult $scanresult
    scanresult=$?
    ./runproj.py --targetdir $TARGETDIR --transferonly --wifidev $WIFIDEV
    if ((0 != $scanresult))
    then
        echo Done
    fi
done
