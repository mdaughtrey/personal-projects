#!/bin/bash

<<<<<<< HEAD
PROJECT=fm131
=======
PROJECT=fm128
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

<<<<<<< HEAD
./rawcap.py --film=8mm --frame=5000 --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT} --startdia 89  --single
=======
./rawcap.py --film=8mm --frame=5000 --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT} --startdia 180  --single
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
