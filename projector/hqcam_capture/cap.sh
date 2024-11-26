#!/bin/bash
#OPTIONS="--immediate --width=2304 --height=1296 --hdr=sensor -e png"
OPTIONS="--immediate --width=2304 --height=1296 -e jpg --awb=indoor --shutter 10000 --saturation 0.0 "
BIN=rpicam-still
OUTFILE=image_$(date +%Y%m%d_%H%M%S).jpg

${BIN} ${OPTIONS} -o ${OUTFILE}
identify ${OUTFILE}

