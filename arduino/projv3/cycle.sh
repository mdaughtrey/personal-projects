#!/bin/bash

BAUD=57600
LINUX_USBDEV=/dev/ttyUSB0
LINUX_STTY=-"stty -F $LINUX_USBDEV $BAUD"
OSX_USBDEV=/dev/tty.usbserial-A601KW2O
OSX_STTY="stty -f $OSX_USBDEV $BAUD"

TRIPLE=y

if [[ `uname` == 'Darwin' ]]
then
    USBDEV=$OSX_USBDEV
    STTY=$OSX_STTY
else
    USBDEV=$LINUX_USBDEV
    STTY=$LINUX_STTY
fi

$STTY
sleep 1
echo t | tr -d '\r\n'  > $USBDEV
sleep 1
exit 0
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
echo x | tr -d '\r\n'  > $USBDEV
