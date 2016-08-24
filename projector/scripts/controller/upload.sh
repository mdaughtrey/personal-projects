#!/bin/bash

SRCDIR=~/Documents/vmshared/scans/Niall/NK0032/100PHOTO/

container=$(basename $SRCDIR)
for filename in ${SRCDIR}/*.JPG
do
    basefile=$(basename $filename)
    curl -i  -F "filename=@${filename}" http://192.168.0.23:5000/upload\?project=uploadtest\&container=${container}\&filename=${basefile}
    exit 0
done

