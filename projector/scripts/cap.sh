#!/bin/bash
set -o xtrace
exec > >(tee cap.log)  2>&1
BIN=/home/mattd/personal-projects/projector/userland/build/bin/raspistill
SPOOLTYPE="SMALL"
FILMTYPE="SUPER8"
PREFRAMES=1
let FRAMES=0
SHUTTER=2,500,970
TARGETDIR=/tmp
MAXINFLIGHT=30
SERIAL_PORT="/dev/ttyUSB0"
SERIAL_SPEED=57600
pushd /home/mattd/personal-projects/projector/raspiraw
camera_i2c
popd

cap()
{
case $SPOOLTYPE in
    SMALL)
        tensionfile="./tension_small.txt"
        let TOTALFRAMES=5000
        ;;
    MID)
        tensionfile="./tension_mid.txt"
        let TOTALFRAMES=20000
        ;;
    LARGE)
        tensionfile="./tension_large.txt"
        let TOTALFRAMES=30000
        ;;
esac

case $FILMTYPE in 
    8MM) 
        PINIT="vd"
        ;;
    SUPER8)
        PINIT="vD"
        ;;
esac

((PREFRAMES*=3))

let count=0
cat $tensionfile | while read tension tcount
do
    echo tension $tension count $tcount
    let THISFRAMES=$((tcount*3))
    PINIT2="${PINIT}-${STEPPERDELAY}s-${tension}T"
    ${BIN} --targetdir $TARGETDIR -mif $MAXINFLIGHT --cycle $((THISFRAMES+PREFRAMES)) -pi $PINIT2 -pf $PREFRAMES -o - -e jpg -t 2 -ss 400 -awb off --exposure off --ISO 100 -v -tr $SHUTTER 2>&1 | tee -a cap.log
    rc=`cat ${TARGETDIR}/exit.code`
    echo rc is $rc
    ((FRAMES+=tcount))
    echo $FRAMES of $TOTALFRAMES
    if [[ "0" != "$rc" ]]
    then
        echo OIT
        touch $TARGETDIR/done.done
        exit 1
    fi
    if ((FRAMES >= TOTALFRAMES))
    then
        echo Done, $FRAMES frames
        touch $TARGETDIR/done.done
        exit 0
    fi
done
exit 0

let TENSION=$((STARTTENSION+TENSTEP))
TENSTEP=${bc<<<"scale=2;${STARTTENSION}-${ENDTENSION}"}
FRAMESTEP=$(bc <<<"scale=2; $TOTALFRAMES/$PERCYCLE")
while (($FRAMES < $TOTALFRAMES))
do
((STEPPERDELAY=1+(FRAMES/PERCYCLE)))
PINIT2="${PINIT}-${STEPPERDELAY}s-${TENSION}T"


echo ${BIN} --targetdir $TARGETDIR -mif $MAXINFLIGHT --cycle $((PERCYCLE+PREFRAMES)) -pi $PINIT2 -pf $PREFRAMES -o - -e jpg -t 2 -ISO 400 -ss 100 -w 3280 -h 2464 -awb sun -v -tr $SHUTTER 2>&1 | tee -a cap.log
rc=$?
if (($TOTALFRAMES-$FRAMES > $PERCYCLE))
then
    if [[ "0" != "$rc" ]]
    then
        echo Early OIT
        exit 1
    fi
fi 

((FRAMES+=PERCYCLE))
done
touch $TARGETDIR/done.done
touch exit.code
}

#for xx in `seq 1000 1000 3000000`; do
reference()
{
    #OUTDIR=/tmp
    OUTDIR=/home/mattd/capture
    for type in raw ppm done bz2; do
        rm ${OUTDIR}/*.${type}
    done
    for xx in `seq 2000 100 4000`; do
        raspiraw --mode 0 --header --i2c 0 --expus $xx --fps 1 -t 250 -sr 1 -o ${OUTDIR}/reference${xx}.raw
        touch ${OUTDIR}/reference${xx}.done
    done

#    raspiraw --mode 0 --header --i2c 0 --expus 1000 --fps 1 -t 250 -sr 1 -o ${OUTDIR}/reference0.raw
#    raspiraw --mode 0 --header --i2c 0 --expus 10000 --fps 1 -t 500 -sr 1 -o ${OUTDIR}/reference1.raw
#    raspiraw --mode 0 --header --i2c 0 --expus 100000 --fps 1 -t 2000 -sr 1 -o ${OUTDIR}/reference2.raw
#    for ii in 0 1 2; do
#        bzip2 ${OUTDIR}/reference${ii}.raw
#        touch ${OUTDIR}/reference${ii}.done
#    done
}

raw()
{
    ./rawcap.py --serial-port $SERIAL_PORT --serial-speed $SERIAL_SPEED
}

case "$1" in 
    cap) cap ;;
    ref) reference ;;
    raw) raw ;;
    *) echo cap/ref; exit 1;;
esac
