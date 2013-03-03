#!/bin/bash

port=/dev/cu.SLAB_USBtoUART

let matrixX=20
let matrixY=7


#set -o xtrace

out ()
{
    echo "$*" > $port
#    echo "$*"
}

pixel ()
{
    let prow=$1
    let pcol=$2
    let plit=$3
    let pindex=$((col/5))

    let pcol=$((pcol - pindex * 5))

    if ((plit))
    then
    out x${pindex}${prow}${pcol}
    else
    out X${pindex}${prow}${pcol}
    fi
    ./sleep100ms
}

draw ()
{
    let rightLimit=$((matrixX-1))
    let leftLimit=0
    let topLimit=0
    let bottomLimit=$((matrixY-1))
    let row=0
    let col=0
    let count=$((matrixX*matrixY))
    let lit=$1
    direction='r'

    while ((count > 0))
    do
        case $direction in 
        r)
            for col in `seq $leftLimit $rightLimit`
            do
                pixel $row $col $lit
                ((count--))
            done
            let col=$rightLimit
            ((topLimit++))
            direction='d'
            ;;

        l) 
            for col in `seq $rightLimit $leftLimit`
            do
                pixel $row $col $lit
                ((count--))
            done
            let col=$leftLimit
            ((bottomLimit--))
            direction='u'
            ;;
        u) 
            for row in `seq $bottomLimit $topLimit`
            do
                pixel $row $col $lit
                ((count--))
            done
            let row=$topLimit
            ((leftLimit++))
            direction='r'
            ;;
        d) 
            for row in `seq $topLimit $bottomLimit`
            do
                pixel $row $col $lit
                ((count--))
            done
            let row=$bottomLimit
            ((rightLimit--))
            direction='l'
            ;;
        esac
   done
}


#init
out c41111
while [[ 0 ]]
do
draw 1
draw 0
done
