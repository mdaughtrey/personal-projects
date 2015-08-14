#!/bin/bash

BASEDIR=~/Video

showname="$@"

if [[ "$showname" != "" && -f "$showname" ]]
then
    cat "$showname" | while read -a params #surl season episode description
    do
	if [[ '#' == ${params[0]} ]]
	then
	    continue
	fi
        if (( ${#params[@]} > 4))
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
        rm xx_${seasonNum}${episodeNum}
        echo ./couchtuner.py -d . -u $url -o xx_${seasonNum}${episodeNum}
        command="mv xx_${seasonNum}${episodeNum} '$outputfile'"
        #echo $command
        echo $command | sh
    done
fi

