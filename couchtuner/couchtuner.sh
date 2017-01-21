#!/bin/bash

BASEDIR=~/Video

showname="$@"
#vendor='allmyvideos.net'
#endor='openload.co'
vendor='thevideo.me'

if [[ "$showname" == "" || ! -f "$showname" ]]
then
    exit 1
fi

cat "$showname" | while read -a params #surl season episode description
do
    if [[ '#' == ${params[0]:0:1} ]]
    then
        continue
    fi
    if [[ '-' == ${params[0]:0:1} ]]
    then
        vendor=${params[1]}
        continue
    fi
    # See if script needs expansion
    if (( ${#params[@]} == 1 ))
    then
        echo params is $params
        read -a p2 <<< ${params//-/ }
        echo p2 is $p2
        p2[0]=$params
        p2[1]=${p2[1]/s/}
        p2[2]=${p2[2]/e/}
        p2[3]=$(echo ${p2[@]:3} | tr ' ' '-' | tr -d '/')
#        for ((ii = 0; ii < ${#p2[@]}; ii++))
#        do
#            echo $ii is ${p2[$ii]}
#        done

#        echo 'p2 is ' ${p2[*]}
        #params[3]=$(echo ${params[@]:3} | sed 's/ /-/;s/\//;')
#        params[3]=${params[3]// /-}
        read -a params <<< ${p2[@]:0:4}
        echo 'params is ' ${params[*]}
        echo 'params length is ' ${#params[@]}
    fi

    if (( ${#params[@]} != 4))
    then
        echo Bad parameter for $params[0]
        exit 1
    fi
    url=${params[0]}
    if [[ '#' == "${url:0:1}" ]]
    then
        continue
    fi
    seasonNum=$(printf "%02u" ${params[1]})
    episodeNum=$(printf "%02u" ${params[2]})
    description=${params[3]}
    episodeDir="$BASEDIR/$showname/Season $seasonNum/"
    if [[ ! -d "$episodeDir" ]]
    then
        mkdir -p "$episodeDir"
    fi
    outputfile="$episodeDir/$showname - s${seasonNum}e${episodeNum} - ${description//-/ }.mp4"
    if [[ -f $outputfile ]]
    then
        echo $outputfile exists, skipping
        continue
    fi
    rm xx_$$_${seasonNum}${episodeNum}
    command="./couchtuner.py -d . -v $vendor -u $url -o xx_${seasonNum}${episodeNum} >> commands.sh"
    echo $command
    command="mv xx_${seasonNum}${episodeNum} '$outputfile'"
    echo $command
    echo $command | sh
done

