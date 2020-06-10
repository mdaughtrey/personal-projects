#!/bin/bash

let count=0

while (($count < 10000))
do
     printf "t4%04d" $count # > /dev/tty.SLAB_USBtoUART
    ((count++))
#    sleep 1
done


