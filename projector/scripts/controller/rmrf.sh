#!/bin/bash

SCANBASE=/mnt/exthd/scans
project=$1
container=$2
filename=`printf '%06u.JPG' $3`
sqlfile=/var/tmp/rfpc.sql

fullfile=$SCANBASE/$project/$container/rawfile/$filename 

if [[ -f "$fullfile" ]]
then
    rm $fullfile
fi

projbase=${SCANBASE}/${project}
cat << SELECT > $sqlfile
DELETE FROM picdata WHERE container='$container' AND rawfile='$filename';
SELECT

#cat $sqlfile
echo .quit | sqlite3 ${SCANBASE}/${project}db -init $sqlfile

