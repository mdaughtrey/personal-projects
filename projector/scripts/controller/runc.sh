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

#        echo "rf: self._scheduleProcessRaw"
#        echo "pc: self._schedulePrecrop"
#        echo "ac: self._scheduleAutocrop"
#        echo "tf: self._scheduleTonefuse"
#        echo "gt: self._scheduleGenTitle"
#        echo "gc: self._scheduleGenContent" ;;
setmode()
{
    mode=$1
    db=${ROOTOFALL}/${PROJECT}/${PROJECT}db
    case "${mode}" in
        init)  -rf ${ROOTOFALL}/${PROJECT} ;;
        del) rm $db ;;
        rf|pc|ac|tf|gt|gc) if [ -f "${db}" ]; then
            sqlite3 $db "delete from taskcontrol; insert into taskcontrol (task) values('"${mode}"');"; fi ;;
    esac
}

case "$1" in 
    run) run ;;
    mode) setmode $2 ;;
    resetpc) sqlite3 $db "update picdata set processing=0, precrop=NULL,precroptag=NULL; .exit" ;;
    resetac) sqlite3 $db "update picdata set processing=0, autocrop=NULL,autocroptag=NULL; .exit" ;;
    *) echo What?
esac
