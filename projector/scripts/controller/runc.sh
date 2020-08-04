#!/bin/bash

<<<<<<< HEAD
PROJECT=fm128
=======
PROJECT=fm131
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
TYPE=8mm
#TYPE=super8
ROOTOFALL=/media/sf_vproj/scans/
IMPORT=/p/vproj/scans
mkdir -p $ROOTOFALL/$PROJECT
db=${ROOTOFALL}/${PROJECT}/${PROJECT}db

run()
{
    JOBMODE=proc
    #JOBMODE=inline
    ./controller.py --jobmode $JOBMODE --project $PROJECT --film $TYPE --saveroot $ROOTOFALL --raw --iimport $IMPORT
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
        im|rf|pc|ac|tf|gt|gc) if [ -f "${db}" ]; then
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

sqlresettf()
{
    sqlite3 $db <<STMTTF
update picdata set processing=0, fused=NULL;
.exit
STMTTF
}

initdb()
{
    cat <<DBINIT
CREATE TABLE picdata ( processing integer, rawfile TEXT, tag TEXT, container TEXT, converted TEXT, convertedtag TEXT, precrop TEXT,
 precroptag TEXT, autocrop TEXT, autocroptag TEXT, fused TEXT, titleframe TEXT);
CREATE TABLE videodata ( container TEXT, processing integer);
CREATE TABLE taskcontrol ( task TEXT);
CREATE UNIQUE INDEX picdata_rawfile_container ON picdata(rawfile, container,tag);
DBINIT

}


rebuild()
{
    ls ${ROOTOFALL}/${PROJECT}/???/rawfile/*.RAW |  while read RAWFILE; do
        name=$(basename $RAWFILE)
        container=$(echo $RAWFILE | egrep -o '/[0-9]{3}/' | tr -d '/')
        echo insert into picdata '(processing,rawfile,tag,container)' values"(0,'"${name:0:6}"','"${name:6:1}"','"${container}"');"
    done
    echo Done rawfile >&2
    ls ${ROOTOFALL}/${PROJECT}/???/converted/*.jpg |  while read JPG; do
        name=$(basename $JPG)
        echo insert into picdata '(processing,converted,convertedtag)' values"(0,'"${name:0:6}"','"${name:6:1}"');"
    done
    echo Done converted >&2
    ls ${ROOTOFALL}/${PROJECT}/???/precrop/*.jpg |  while read file; do
        name=$(basename $file)
        echo insert into picdata '(processing,precrop,precroptag)' values"(0,'"${name:0:6}"','"${name:6:1}"');"
    done
    echo Done precropped >&2
    ls ${ROOTOFALL}/${PROJECT}/???/autocrop/*.jpg |  while read file; do
        name=$(basename $file)
        echo insert into picdata '(processing,autocrop,autocroptag)' values"(0,'"${name:0:6}"','"${name:6:1}"');"
    done
    echo Done autocropped >&2
    ls ${ROOTOFALL}/${PROJECT}/???/fused/*.jpg |  while read file; do
        name=$(basename $file)
        echo insert into picdata '(processing,fused)' values"(0,'"${name:0:6}"');"
    done
    echo Done fused >&2
}

rebuild2()
{
    readarray -t raw <<<$(ls ${ROOTOFALL}/${PROJECT}/???/rawfile/*.RAW)
#    readarray -t converted <<<$(ls ${ROOTOFALL}/${PROJECT}/???/converted/*.jpg)
#    readarray -t precrop <<<$(ls ${ROOTOFALL}/${PROJECT}/???/precrop/*.jpg)
#    readarray -t autocrop <<<$(ls ${ROOTOFALL}/${PROJECT}/???/autocrop/*.jpg)
#    readarray -t fused <<<$(ls ${ROOTOFALL}/${PROJECT}/???/fused/*.jpg)

    for ii in ${raw[@]}; do
        file=$(basename $ii})
        name=${file:0:6}
        tag=${file:6:1}
        container=$(echo ${ii} | egrep -o '/[0-9]{3}/' | tr -d '/')
        echo insert into picdata '(processing,rawfile,tag,container,converted,convertedtag,precrop,precroptag,autocrop,autocroptag,fused)' values"(0,'"${name}"','"${tag}"','"${container}"','"${name}"','"${tag}"','"${name}"','"${tag}"','"${name}"','"${tag}"','"${name}"');"
    done
}


case "$1" in 
    run) run ;;
    mode) setmode $2 ;;
<<<<<<< HEAD
    title) ../gentitle.sh -r ${ROOTOFALL} -p ${PROJECT} -u ac ;;
    gen) use=${2:-ac}
        ../gencontent.sh -r ${ROOTOFALL} -p ${PROJECT} -u ${use} ;;
=======
    title) bash -x ../gentitle.sh -r ${ROOTOFALL} -p ${PROJECT} -u ac ;;
    gen) use=${2:-ac}
        ../gencontent.sh -r ${ROOTOFALL} -p ${PROJECT} -u ${use}  ;;
>>>>>>> 39da4ec1825214e7f11a6eaae2bb8cee88ff1f06
        #../gencontent.sh -r ${ROOTOFALL} -p ${PROJECT} -u ${use} >/dev/null 2>&1;;
    initdb) initdb | sqlite3 ${ROOTOFALL}/${PROJECT}/${PROJECT}db ;;
    resetpc) sqlreset precrop ;;
    resetac) sqlreset autocrop ;;
    resettf) sqlresettf ;;
    resetrf) sqlreset converted ;;
    rebuild2) rebuild2 ;;
    rebuild) rm /tmp/rebuilddb
        initdb > rebuild.txt
        echo Init
        rebuild2 >> rebuild.txt
        echo Rebuild.txt
        echo ".exit" >> rebuild.txt
        echo Rebuilding
        cat rebuild.txt | sqlite3 /tmp/rebuilddb
        echo Moving
        mv /tmp/rebuilddb ${ROOTOFALL}/${PROJECT}/${PROJECT}db
        echo Done
        ;;
    *) echo What?
esac
