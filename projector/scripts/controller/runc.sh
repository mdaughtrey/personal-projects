#!/bin/bash

PROJECT=test
#TYPE=8mm
TYPE=super8
ROOTOFALL=/media/sf_vproj/scans/
#if[[! -d "$ROOTOFALL" ]]
#then
	mkdir -p $ROOTOFALL/$PROJECT

#fi

#JOBMODE=inline
JOBMODE=proc
./controller.py --jobmode $JOBMODE --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw
#./controller.py --jobmode uploadonly --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw
