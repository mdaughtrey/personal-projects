#!/bin/bash
BIN=/home/mattd/userland/build/bin/raspistill
PREFRAMES=6
let FRAMES=12000
#let FRAMES=400
SHUTTER=1,5,10
PINIT_8MM="dt"
PINIT_SUPER8="Dt"

#gdb --args ${BIN} --cycle $(((FRAMES * 3) + $PREFRAMES)) -pi $PINIT_8MM -pf $PREFRAMES -o - -e jpg -t 2 -ISO 100 -ss 100 -w 3280 -h 2464 -awb sun -v -rot 180 -tr $SHUTTER 2>&1 | tee log.txt
${BIN} --cycle $(((FRAMES * 3) + $PREFRAMES)) -pi $PINIT_SUPER8 -pf $PREFRAMES -o - -e jpg -t 2 -ISO 100 -ss 100 -w 3280 -h 2464 -awb sun -v -rot 180 -tr $SHUTTER 2>&1 | tee log.txt

