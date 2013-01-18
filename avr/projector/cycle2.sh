#!/bin/bash

let numframes=$1
let count=0

./init.sh
export USBDEV

#set -o xtrace
while ((count < numframes))
do
./l
sleep 1
./c
sleep 1
#./L
./n
((count++))
echo $(date +%T) Done $count of $numframes
done
./x

