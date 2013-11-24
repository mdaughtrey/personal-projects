#!/bin/bash

USBDEV=/dev/ttyUSB0
BAUD=57600

stty -F $USBDEV $BAUD


let count=$1

while ((count > 0))
do
    echo $count frames remaining
    echo C > $USBDEV
    sleep 2
    ((count--))
done
