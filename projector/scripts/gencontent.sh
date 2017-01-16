#!/bin/bash

SEM=sem

while getopts "p:r:c:" OPT
do
    case $OPT in
        p) project=$OPTARG ;;
        r) fileroot=$OPTARG ;;
        c) container=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

export imgroot=$fileroot/$project/$container/fused
export projroot=$fileroot/$project

if [[ "" == $project || "" == $fileroot || "" == $container ]]
then
    echo Some parameter is missing
    exit 1
fi

getImages()
{
    IFS=\|
    select="select fused from picdata where fused is not NULL and container='$container' order by fused"
    sqlite3 -list $fileroot/${project}db "$select" | uniq | while read filename
    do
        echo $imgroot/$filename
    done
}

genyuvstream()
{
    getImages > $imgroot/contentlist.txt

    mplayer -msglevel all=6 -lavdopts threads=`nproc` \
        mf://@$imgroot/contentlist.txt \
        -vf scale=800:600 \
    	-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub \
        -vo yuv4mpeg:file=$imgroot/content.yuv
}

converttoavi()
{
    if [[ ! -f "$imgroot/content.yuv" ]]
    then
        genyuvstream
    fi
    cat $imgroot/content.yuv | yuvfps -v 0 -r 18:1 -v 1 | \
        avconv -loglevel info -i - -vcodec rawvideo -y $imgroot/content.avi
}

converttoavi

