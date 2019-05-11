#!/bin/bash

PROJECT=geo6_2
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
    if [ "" = "${mode}" ]; then
      echo "rf: self._scheduleProcessRaw"
      echo "pc: self._schedulePrecrop"
      echo "ac: self._scheduleAutocrop"
      echo "tf: self._scheduleTonefuse"
      echo "gt: self._scheduleGenTitle"
      echo "gc: self._scheduleGenContent"
    fi
    if [ "init" == "${mode}" ]; then
        rm -rf ${ROOTOFALL}/${PROJECT}
        return
    fi
    if [ "del" = "${mode}" ]; then
        rm $db
        return
    fi
    if [ -f "${db}" ]; then
        sqlite3 $db "delete from taskcontrol; insert into taskcontrol (task) values('"${mode}"');"
    fi
}

case "$1" in 
    run) run ;;
    mode) setmode $2 ;;
    *) echo What?
esac
