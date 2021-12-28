#!/bin/bash

ii=43
#for ii in 43 44 45 46 47
#do
    #PROJECT="interval_$ii"
    PROJECT="up1turn"
    sudo ./hqcap1.py  --film=8mm --dev=/mnt/exthd --dir /mnt/exthd/${PROJECT}  --single --picameracont \
    --frameinterval $ii

    #--frames 200 --frameinterval $ii
#done



#./hqcap1.py --film=8mm --dev=/dev/mmcblk0p2 --dir /var/tmp/${PROJECT}  --single --picameracont # --raspid ${CHILD}
