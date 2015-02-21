#!/bin/bash

let numframes=$1
let count=0

./init.sh
export USBDEV

#set -o xtrace
while ((count < numframes))
do
./c
sleep 1
./n
sleep 1
./l
sleep 1
./L
sleep 1
((count++))
echo $(date +%T) Done $count of $numframes
done
./x

