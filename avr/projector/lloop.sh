#!/bin/bash

let numframes=$1
let count=0

./init.sh

while ((count < numframes))
do
./l
sleep 1
./L
sleep 1
((count++))
echo Done $count of $numframes
done

