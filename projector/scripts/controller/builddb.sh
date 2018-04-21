#!/bin/bash

SCANBASE=/home/mattd/scans/nk
project=$1
if [[ "$project" == "" ]]
then
    echo No project
fi
projbase=${SCANBASE}/${project}
sqlfile=/var/tmp/builddb.sql
db=${projbase}/${project}db

(cat <<XYB
CREATE TABLE picdata ( processing integer, rawfile TEXT, container TEXT, precrop TEXT,
    autocrop TEXT, fused TEXT, titleframe TEXT);
CREATE TABLE videodata ( container TEXT, processing integer);
CREATE TABLE taskcontrol ( task TEXT );
CREATE UNIQUE INDEX picdata_rawfile_container ON picdata(rawfile, container);

XYB
) > $sqlfile

find $projbase -name '*.JPG' | grep rawfile | while read afile
do
    afile=${afile/$projbase/}
    afile=${afile//\// }
    echo $afile | while read container type filename
    do
#        echo Container $container Type $type Filename $filename
        (echo "INSERT INTO picdata (processing,container,rawfile) values(0,'"$container"','"$filename"');") >> $sqlfile
    done
done

if [[ -f "$db" ]]
then
    rm $db
fi

sqlite3 ${db} -init $sqlfile
