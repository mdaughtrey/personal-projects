#!/bin/bash

export USBDEV=/dev/ttyUSB0
#export USBDEV=/dev/ttyUSB0

stty -F $USBDEV 38400
stty -F $USBDEV 38400
$@
