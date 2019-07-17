#!/bin/bash

pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --film=8mm --frame=3500 --dev=/mnt/exthd --project 8mm2 $@
