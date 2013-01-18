#!/bin/bash

let numframes=$1
let count=0

./init.sh

while ((count < numframes))
do
./n
./L

sleep 2
((count++))
echo Done $count of $numframes
done

