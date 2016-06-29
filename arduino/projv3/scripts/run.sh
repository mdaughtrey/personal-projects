#!/bin/bash

NUMFRAMES=200
MNTPOINT=/Volumes/imageinput
SERVER=smb://GUEST:@nas-36-FE-22%20%28SMB%29._smb._tcp.local/imageinput

if [[ ! -d $MNTPOINT ]]
then
    open $SERVER
fi

arg=${1:-capture}

if [[ "capture" == "$arg" ]]
then
    exec ./runproj.py --targetdir ./tmp --numframes $NUMFRAMES --pretension 20 --mode 8mm 
else
    exec ./runproj.py --targetdir ./tmp --transferonly
fi
