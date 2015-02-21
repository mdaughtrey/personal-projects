#!/bin/bash

USBDEV=/dev/ttyUSB0
BAUD=57600
TRIPLE=yu

stty -F $USBDEV $BAUD


echo y | tr -d '\r\n'  > $USBDEV
let count=$1

while ((count > 0))
do
    echo $count frames remaining
    if [[ "$TRIPLE" == "" ]]
    then
        echo C | tr -d '\r\n'  > $USBDEV
        sleep 2
    else
        echo 3 | tr -d '\r\n'  > $USBDEV
        sleep 4
    fi
    ((count--))
done
echo Y | tr -d '\r\n'  > $USBDEV
