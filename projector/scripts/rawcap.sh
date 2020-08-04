#!/bin/bash

PROJECT=fm131
pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --film=8mm --frame=5000 --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT} --startdia 140  --single
