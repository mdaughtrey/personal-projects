#!/bin/bash

for ii in 43 44 45 46 47
do
    PROJECT="interval_$ii"
    sudo ./hqcap1.py  --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT}  --single --picameracont \
    --frames 200 --frameinterval $ii
done



#./hqcap1.py --film=8mm --dev=/dev/mmcblk0p2 --dir /var/tmp/${PROJECT}  --single --picameracont # --raspid ${CHILD}
