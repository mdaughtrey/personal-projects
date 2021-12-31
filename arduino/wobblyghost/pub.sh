#!/bin/bash

HOST=localhost

loop()
{
    for (( ii=0; ii<65535; ii+=100 ))
    do
        xx=$((ii<<16|65535))
        printf '%08x\r\n' $xx
        mosquitto_pub -h ${HOST} -p 1883 -t '/wg483fda48927f/ping'  -m $xx
        sleep 5 
    done
}

lrandom()
{
    while [[ 0 ]]; do
        vv=$(((65536/6) * ($RANDOM % 7)))
        xx=$(($vv <<16|65535))
        printf '%08x\r\n' $xx
        mosquitto_pub -h ${HOST} -p 1883 -t '/WG483FDA48927F/ping'  -m $xx
        sleep 10
    done
}

case "$1" in 
    l0) mosquitto_pub -h ${HOST} -p 1883 -t '/wg483fda48927f/led'  -m '0' ;; 
    l1) mosquitto_pub -h ${HOST} -p 1883 -t '/wg483fda48927f/led'  -m '1' ;; 
    p) mosquitto_pub -h ${HOST} -p 1883 -t '/wg483fda48927f/ping'  -m '238085782' ;; 
    pl) loop ;;
    pr) lrandom ;;
    *)
esac

