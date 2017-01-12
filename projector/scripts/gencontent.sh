#!/bin/bash

SEM=sem

while getopts "p:r:" OPT
do
    case $OPT in
        p) project=$OPTARG ;;
        r) fileroot=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

export imgroot=$fileroot/$project
export projroot=$fileroot/$project

if [[ "" == $project || "" == $fileroot ]]
then
    echo Some parameter is missing
    exit 1
fi

getImages()
{
    IFS=\|
    select="select container,fused from picdata where fused is not NULL order by container,fused"
    sqlite3 -list $fileroot/${project}db "$select" | uniq | while read container filename
    do
        echo $fileroot/$project/$container/fused/$filename
    done
}

genyuvstream()
{
    getImages > $projroot/contentlist.txt

    mplayer -msglevel all=6 -lavdopts threads=`nproc` \
        mf://@$projroot/contentlist.txt \
        -vf scale=800:600 \
    	-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub \
        -vo yuv4mpeg:file=$projroot/content.yuv
}

converttoavi()
{
    if [[ ! -f "$projroot/content.yuv" ]]
    then
        genyuvstream
    fi
    cat $projroot/content.yuv | yuvfps -v 0 -r 18:1 -v 1 | \
        avconv -loglevel info -i - -vcodec rawvideo -y $projroot/content.avi
}

converttoavi

