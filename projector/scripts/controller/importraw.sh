#!/bin/bash

SCANBASE=/media/sf_vproj/scans/
project=$1
if [[ "$project" == "" ]]
then
    echo No project
fi
projbase=${SCANBASE}/${project}
sqlfile=/tmp/builddb.sql
db=${projbase}/${project}db

(cat <<XYB
CREATE TABLE picdata (processing integer, rawfile TEXT, tag TEXT, container TEXT, converted TEXT,
    convertedtag TEXT, precrop TEXT, precroptag TEXT, autocrop TEXT, autocroptag TEXT, fused TEXT, titleframe TEXT);
CREATE TABLE videodata ( container TEXT, processing integer);
CREATE TABLE taskcontrol ( task TEXT );
CREATE UNIQUE INDEX picdata_rawfile_container ON picdata(rawfile, container);

XYB
) > $sqlfile

ls $projbase/???/rawfile/*.raw | while read afile
do
    filenum=$(basename $afile | sed 's/[abc]\.raw//')
    readarray -td/ dirpath<<<$(dirname $afile)
    container=${dirpath[-2]}
    (echo "INSERT INTO picdata (processing,container,rawfile,tag) values(0,'"$container"','"$filenum"','"a"');") >> $sqlfile
done

exit 0

if [[ -f "$db" ]]
then
    rm $db
fi

sqlite3 ${db} -init $sqlfile
