#!/bin/bash

PROJECT=test2
#TYPE=8mm
TYPE=super8
ROOTOFALL=/media/sf_vproj/scans/
mkdir -p $ROOTOFALL/$PROJECT
db=${ROOTOFALL}/${PROJECT}/${PROJECT}db

run()
{
    JOBMODE=proc
    #JOBMODE=inline
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
    case "${mode}" in
        init)  -rf ${ROOTOFALL}/${PROJECT} ;;
        del) rm $db ;;
        rf|pc|ac|tf|gt|gc) if [ -f "${db}" ]; then
            sqlite3 $db "delete from taskcontrol; insert into taskcontrol (task) values('"${mode}"');"; fi ;;
    esac
}

#    resetpc) resetpc ;;sqlite3 $db "update picdata set processing=0, precrop=NULL,precroptag=NULL;\\r .exit" ;;
#    resetac) sqlite3 $db "update picdata set processing=0, autocrop=NULL,autocroptag=NULL;\\r .exit" ;;

#    $(echo <<STATEMENT
#        update picdata set processing=0, ${1}=NULL,${1}tag=NULL;
#        .exit
#    STATEMENT
#    )  

sqlreset()
{
    sqlite3 $db <<STMT
update picdata set processing=0, ${1}=NULL,${1}tag=NULL;
.exit
STMT
}


case "$1" in 
    run) run ;;
    mode) setmode $2 ;;
    resetpc) sqlreset precrop ;;
    resetac) sqlreset autocrop ;;
    resettf) sqlreset fused ;;
    *) echo What?
esac
