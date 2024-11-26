#!/bin/bash
#

#OPTIONS="--immediate --width=2304 --height=1296 --hdr=sensor -e png"
EXT=jpg
OPTIONS="--immediate --width=2304 --height=1296 -e ${EXT} --awb=indoor"
BIN=rpicam-still

test_exposure()
{
    OPTIONS2=$1
    TAG=$2
    for ((s=5000; s<200000; s+=5000)); do
        OUTFILE=${s}${TAG}.${EXT}
        command="${BIN} ${OPTIONS} ${OPTIONS2}  --shutter ${s} --output ${OUTFILE}"
        $command
        sleep 2
    done
}


test_hdr()
{
    for HDR in sensor single-exp; do
        test_exposure "--hdr ${HDR}" _${HDR}
    done
}

test_fuse()
{
    enfuse --output enfused.jpg 36000.jpg 66000.jpg 105000.jpg 
    convert 36000.jpg 66000.jpg +append row1.jpg 
    convert 105000.jpg enfused.jpg +append row2.jpg
    convert row1.jpg row2.jpg -append grid.jpg
}

case "$1" in 
	exp) test_exposure ;;
    hdr) test_hdr ;;
    fuse) test_fuse ;;
	*) echo What?
esac
