#!/bin/bash

#set +o noexec

PROJECT=hq101
#TYPE=super8
ROOTOFALL=/media/sf_vproj/scans/
IMPORT=/p/vproj/scans
mkdir -p $ROOTOFALL/$PROJECT

import()
{
    let dirnum=0
    while [[ /bin/true ]]; do
        targetdir=${ROOTOFALL}${PROJECT}"/"$(printf '%03d' ${dirnum})
	echo Copying to ${targetdir}
        sourcefile=$(printf '00%d????.rgb' ${dirnum})
        mkdir -p  ${targetdir}
        rsync -v -u mattd@hqcam:/mnt/exthd/${PROJECT}/${sourcefile} ${targetdir}
        if [[ -z "$(ls -A ${targetdir})" ]]; then
            rm -rf ${targetdir}
            echo Import complete
            exit 0
        fi
        ((dirnum+=1))
    done
    #echo scp mattd@hqcam:/mnt/exthd/${PROJECT}/${sourcefile}*.rgb ${targetdir}

}

case "$1" in 
    title) ../gentitle.sh -r ${ROOTOFALL} -p ${PROJECT} -u ac ;;
    gen) use=${2:-ac}
        bash -x ../gencontenthq.sh -h -r ${ROOTOFALL} -p ${PROJECT} -u ${use} ;;
    import) import ;;
    run) ./hqtest.py --project ${PROJECT} --root ${ROOTOFALL} --format png ;;
    test) ffmpeg -loglevel verbose -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;
    *) echo What?
esac
