#!/bin/bash

port=/dev/cu.SLAB_USBtoUART

#stty -f $port clocal -crtscts
#cat $port &

#set -o xtrace

out ()
{
    echo "$*" > $port
}

palette ()
{
    let count=0
    while ((count < 100))
    do
        out l4$1
        out l4$2
        ((count++))
    done
}


while [[ 0 ]]
do
out l40000
out t4cats
sleep 3
out t4CATS
sleep 3
out 't4C   '
sleep 1
out 't4 A  '
sleep 1
out 't4  T '
sleep 1
out 't4   S'
sleep 2
out 't4cats'

let count=0
while ((count < 4))
do
    out 'l40101'
    sleep 1
    out 'l41010'
    sleep 1
    ((count++))
done

out 'l40000'
out 't4 we '
out 't4luvz'
out 't4cats'

palette 0000 1000
palette 1000 1100
palette 1100 1110
palette 1110 1111

done
