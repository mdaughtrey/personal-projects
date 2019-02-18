#!/bin/bash

pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --frame=1000 --dev=/mnt/extfd $@
