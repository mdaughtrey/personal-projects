#!/bin/bash

PROJECT=$1
#pushd /home/mattd/personal-projects/projector/raspiraw
#camera_i2c
#popd
#raspistill -v -o %06d.png -e png  -q 100 -t 0 -s  -fs 0 \
#--nopreview --nopreview --exposure night  --awb off --shutter 1000000  &
#
#CHILD=$!

./hqcap.py --film=8mm --frame=20 --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT} --startdia 140  --single
