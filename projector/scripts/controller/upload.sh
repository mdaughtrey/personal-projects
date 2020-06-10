#!/bin/bash

set -o xtrace

SERVER=192.168.0.18:5000
SRCDIR=~/Documents/vmshared/scans/Niall/NK0032/100PHOTO/

let count=0
let limit=${1:-1}
container=$(basename $SRCDIR | cut -b1-3)
for filename in ${SRCDIR}/*.JPG
do
    basefile=$(basename $filename)
	filenum=$(echo $basefile | cut -b5-8)
    filenum=$(printf '%06u' $((10#$filenum)))
    curl -i  -F "filename=@${filename}" http://${SERVER}/upload\?project=uploadtest\&container=${container}\&filename=${filenum}.JPG
    ((count++))
    if ((count > $limit))
    then
        exit 0
    fi
done

