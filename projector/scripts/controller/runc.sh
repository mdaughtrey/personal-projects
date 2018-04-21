#!/bin/bash

PROJECT=blankref
#TYPE=8mm
TYPE=super8
ROOTOFALL=/home/mattd/scans/
#if[[! -d "$ROOTOFALL" ]]
#then
	mkdir -p $ROOTOFALL/$PROJECT

#fi

#./controller.py --jobmode proc --project blister --film super8 #--worker pc --worker ac  # --worker tf --worker gc
#./controller.py --jobmode proc --project $PROJECT --film super8
./controller.py --jobmode proc --project $PROJECT --film $TYPE --saveroot $ROOTOFALL
