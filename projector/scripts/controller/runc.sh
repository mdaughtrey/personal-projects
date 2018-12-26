#!/bin/bash

PROJECT=test
#TYPE=8mm
TYPE=super8
ROOTOFALL=/media/sf_vproj/scans/
#if[[! -d "$ROOTOFALL" ]]
#then
	mkdir -p $ROOTOFALL/$PROJECT

#fi

#./controller.py --jobmode proc --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw
./controller.py --jobmode inline --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw
