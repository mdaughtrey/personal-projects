#!/bin/bash

#set -o xtrace

for ff in podl_*.bin
do
    echo $ff
    cat $ff | nc -vv -q 2 -u 127.0.0.1 10000 
    sleep 3
done
