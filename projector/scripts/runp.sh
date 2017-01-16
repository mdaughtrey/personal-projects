#!/bin/bash


cycles=200

while [[ "$cycles" != "0" ]]
do
    ./pcontrol.py --filmtype s8 --mode pipeline --cycles 100
    if [[ "$?" != "0" ]]
    then
        echo Something crapped out
        exit 0
    fi
    ((cycles-=100))
done
echo Done
exit 0
