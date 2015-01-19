#!/bin/bash

showname=$1

if [[ "$showname" != "" && -f "$showname" ]]
then
    cat $showname | while read url outname
    do
        sem -j4 ./couchtuner.py -d ~/Video -s $showname -u $url -o $outname
    done
    sem --wait
fi

