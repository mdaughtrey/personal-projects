#!/bin/bash

port=/dev/cu.SLAB_USBtoUART

#stty -f $port clocal -crtscts
#cat $port &

#set -o xtrace

out ()
{
    echo "$*" > $port
}


rollit ()
{
    let ii=0
    while ((ii < 7))
    do
        out u4$ii$(((ii+1)%7))$(((ii+2)%7))$(((ii+3)%7))
            ((ii++))
        ./sleep100ms 
    done

}

out t4____

while [[ 0 ]]
do
    rollit
done
