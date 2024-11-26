#!/bin/bash

# mechtest13 42 6/7
#1 of 9INTERVALS=42,43,43,43,43,43,43,43,43
INTERVALS=42,43,43,43,43,43,43,43,43,42,43,43,43,43,43,43,43,43,43

# mechtest14 32 2/13
#INTERVALS=43

PROJECT=hd0

run()
{
    sudo ./hqcap1.py \
        --film=8mm \
        --dev=/mnt/exthd \
        --dir /mnt/exthd/${PROJECT} \
        --single --picameracont \
        --intervals=${INTERVALS} \
        --frames=100 \
        --nocam
#        --draft 
}

batch()
{
    for ((ii=0;ii<10;ii++))
    do
        intervals="42,43,43,43"
        for ((jj=0;jj<${ii};jj++))
        do
            intervals="${intervals},43"
        done
        title=${intervals//,/_}
        echo $title
#        PROJECT=${title} INTERVALS=${intervals} run
    done

#    titles=(mt_42 mt_42_42 mt_42_42_43 mt_42_43 mt_43 mt_43_43 mt_42_43_43)
#    intervals=(42 42,42 42,42,43 42,43 43 43,43 42,43,43)
#
#    for ((ii=0; ii<${#titles[@]};ii++))
#    do
#        PROJECT=${titles[$ii]} INTERVALS=${intervals[$ii]} run
#    done
}

run
#batch
