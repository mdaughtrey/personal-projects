#!/bin/bash

pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

./rawcap.py --frames=10 --dev=/mnt/extfd --nofilm $@
