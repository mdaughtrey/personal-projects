#!/bin/bash

pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --frame=2000 --dev=/mnt/extfd $@
