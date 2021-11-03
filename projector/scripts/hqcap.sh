#!/bin/bash

PROJECT=$1
./hqcap1.py --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT}  --single --picameracont --frames 200 # --raspid ${CHILD}
#./hqcap1.py --film=8mm --dev=/dev/mmcblk0p2 --dir /var/tmp/${PROJECT}  --single --picameracont # --raspid ${CHILD}
