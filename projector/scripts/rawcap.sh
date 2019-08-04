#!/bin/bash

pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --film=8mm --frame=50000 --dev=/mnt/exthd --dir /mnt/exthd/fm104 $@
