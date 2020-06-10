#!/bin/bash

SOURCE=/p/vproj/import/fm128b
DEST=/p/vproj/scans/fm128


if [[ -d "${DEST}" ]]; then
    let diri=$((10#$(ls ${DEST} | sort | tail -1)))
else    
    let diri=1
    echo mkdir -p ${DEST}/001/rawfile
fi

echo diri $diri

if [[ -d "${DEST}/$(printf '%03u' ${diri})" ]]; then
    #dd=$(basename $(ls ${DEST}/$(printf "%03u" $diri)/rawfile/*.raw | sort -n | tail -1 | sed 's/a\.raw//'))
    #let destcount=10#$dd
    let destcount=10#$(basename $(ls ${DEST}/$(printf "%03u" $diri)/rawfile/*.raw | sort -n | tail -1 | sed 's/a\.raw//'))
    ((destcount++))
else
    let destcount=0
fi

let filecount=0

echo destcount $destcount filecount $filecount


for ff in $( ls ${SOURCE}/*.raw | sort); do
    cp ${ff} ${DEST}/$(printf "%03u" ${diri})/rawfile/$(printf "%06ua.raw" ${destcount})

    ((filecount++))
    ((destcount++))
    if ((destcount == 1000)); then
        ((diri++))
        let destcount=0
        mkdir -p ${DEST}/$(printf "%03u" ${diri})/rawfile
    fi
done
