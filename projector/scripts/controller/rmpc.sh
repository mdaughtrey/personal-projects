#!/bin/bash

SCANBASE=/media/sf_vmshared/scans
project=$1
container=$2
filename=`printf '%06u.JPG' $3`
sqlfile=/var/tmp/rmpc.sql

fullfile=$SCANBASE/$project/$container/precrop/$filename 

if [[ ! -f "$fullfile" ]]
then
    echo $fullfile does not exist
    exit 1
fi

projbase=${SCANBASE}/${project}

statement="DELETE FROM picdata WHERE container='"$container"' AND precrop='"$filename"';"
rm $fullfile

echo $statement > $sqlfile
echo .quit | sqlite3 ${projbase}/${project}db -init $sqlfile

