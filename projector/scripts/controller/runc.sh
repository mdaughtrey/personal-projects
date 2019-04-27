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
run()
{
    JOBMODE=proc
    ./controller.py --jobmode $JOBMODE --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw 
    #./controller.py --jobmode uploadonly --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw
}

setmode()
{
    mode=$1
    db=${ROOTOFALL}/${PROJECT}/${PROJECT}db
    sqlite3 $db "delete from taskcontrol; insert into taskcontrol (task) values('"${mode}"');"
}

case "$1" in
    run) run ;;
    mode) setmode $2 ;;
    *) echo What? ;;
esac
