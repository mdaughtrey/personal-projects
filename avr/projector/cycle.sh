#!/bin/bash

#set -o xtrace
device=/dev/ttyUSB0
#device=/dev/null

count=${1:-3250}
while [[ $count -gt 0 ]]; do
#echo "lamp on"
echo "l" > ${device}
sleep 1
#echo "click"
echo "c" > ${device}
sleep 1
echo "Ln" > ${device}
#echo "lamp off, advance"
sleep 1
let count=$count-1
echo $count frames remaining
done
