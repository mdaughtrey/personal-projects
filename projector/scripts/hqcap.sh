#!/bin/bash

PROJECT=$1
#pushd /home/mattd/personal-projects/projector/raspiraw
#camera_i2c
#popd
#raspistill -v -o %06d.png -e png  -q 100 -t 0 -s  -fs 0 \
#--nopreview --nopreview --exposure night  --awb off --shutter 1000000  &
#

# TODO search for the latest image file
#/usr/bin/raspistill -v -e png -q 100 -t 0 -s -fs 0 --nopreview --exposure night --awb off --shutter 10000 -o  /mnt/exthd/${PROJECT}/'%06d.png' &
#CHILD=$!
#echo Child process is ${CHILD}
#    outfile = "{}".format("{:s}/{:s}{:06d}a.png".format(config.dir, config.prefix, framenum))
#    args = ['/usr/bin/raspistill','-v',
#        '-e','png',
#        '-q','100',
#        '-t','0',
#        '-s','-fs','0',
#        '--nopreview',
#        '--exposure','night',
#        '--awb','off',
#        '--shutter','10000',
#        '-o','outfile']

#./hqcap.py --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT} --startdia 55  --single --picameracont # --raspid ${CHILD}
./hqcap.py --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT}  --single --picameracont # --raspid ${CHILD}
#./hqcap3.py --frames=20 --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT}  --noled --picameracont
#echo Killing child process
#kill $(jobs -p)
