#!/bin/bash

SCANBASE=/media/sf_vmshared/scans
project=$1
shift
container=$1
shift
files=$@
sqlfile=/var/tmp/rmpc.sql

projbase=${SCANBASE}/${project}

statement="DELETE FROM picdata WHERE container='"$container"' AND precrop IN ("

for file in $files
do
    statement=$statement"'"$file"',"
    echo rm ${SCANBASE}/${project}/${container}/precrop/$file
done

echo -n ${statement:0:${#statement}-1} > $sqlfile
(echo ");") >> $sqlfile
sqlite3 ${projbase}/${project}db -init $sqlfile

