#!/bin/bash
BIN=/home/mattd/userland/build/bin/raspistill
SPOOLTYPE="SMALL"
FILMTYPE="SUPER8"
PREFRAMES=3
let FRAMES=0
#let FRAMES=4000
SHUTTER=4000,10000,60000
TARGETDIR=/tmp
MAXINFLIGHT=30

case $SPOOLTYPE in
    SMALL)
        tensionfile="./tension_small.txt"
        #read -a tensions <<<$(<./tension_small.txt)
        let TOTALFRAMES=3500
        let PERCYCLE=500
        ;;
    LARGE)
        let TOTALFRAMES=3500
        let PERCYCLE=500
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

#((TOTALFRAMES*=3))
((PREFRAMES*=3))
#((PERCYCLE*=3))

echo PREFRAMES $PREFRAMES PERCYCLE $PERCYCLE
rm log.txt

cat $tensionfile | while read tension tcount
do
    echo tension $tension count $tcount
    let THISFRAMES=$((tcount*3))
    PINIT2="${PINIT}-${STEPPERDELAY}s-${tension}T"
    ${BIN} --targetdir $TARGETDIR -mif $MAXINFLIGHT --cycle $((THISFRAMES+PREFRAMES)) -pi $PINIT2 -pf $PREFRAMES -o - -e jpg -t 2 -ISO 400 -ss 100 -w 3280 -h 2464 -awb sun -v -tr $SHUTTER 2>&1 | tee -a log.txt
    rc=$?
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

#let TENSTEP=$(((STARTTENSION-ENDTENSION)/FRAMESTEP))
let TENSION=$((STARTTENSION+TENSTEP))
#TENSTEP=${bc<<<"scale=2;($STARTTENSION-$ENDTENSION)/($TOTALFRAMES/$PERCYCLE)"}
TENSTEP=${bc<<<"scale=2;${STARTTENSION}-${ENDTENSION}"}
FRAMESTEP=$(bc <<<"scale=2; $TOTALFRAMES/$PERCYCLE"}
while (($FRAMES < $TOTALFRAMES))
do
((STEPPERDELAY=1+(FRAMES/PERCYCLE)))
PINIT2="${PINIT}-${STEPPERDELAY}s-${TENSION}T"


echo ${BIN} --targetdir $TARGETDIR -mif $MAXINFLIGHT --cycle $((PERCYCLE+PREFRAMES)) -pi $PINIT2 -pf $PREFRAMES -o - -e jpg -t 2 -ISO 400 -ss 100 -w 3280 -h 2464 -awb sun -v -tr $SHUTTER 2>&1 | tee -a log.txt
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
