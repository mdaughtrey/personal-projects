#!/bin/bash

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

SEM=sem

while getopts "p:r:" OPT
do
    case $OPT in
        p) project=$OPTARG ;;
        r) fileroot=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

export imgroot=$fileroot/$project/
#export projroot=$fileroot/$project

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
        echo ${imgroot}/${container}/fused/${filename}
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

processyuv()
{
    if [[ ! -f "$imgroot/content.yuv" ]]
    then
        genyuvstream
    fi
    if [[ ! -f "$imgroot/content.avi" ]]
    then
        cat $imgroot/content.yuv | yuvfps -v 0 -r 18:1 -v 1 | \
            avconv -loglevel info -i - -vcodec rawvideo -y $imgroot/content.avi
    fi
	
    avsscript=/tmp/interpolate.avs
    echo "film=\"${imgroot}/content.avi\"" > $avsscript
	#echo -n "film=\"Y:\\\\`basename $PWD`\\\\rawframes.avi\"" > $avsscript
	echo "result=\"result1\" # specify the wanted output here" >> $avsscript
    cat $MYDIR/pp_interpolate2.avs >> $avsscript
    #wine Z:\\mnt\\imageinput\\software\\avs2yuv\\avs2yuv.exe $avsscript - > $imgroot/out.yuv
    WINEDLLPATH=~/.wine/drive_c/windows/system32 
#    export WINEDEBUG=trace+all
    wine32 /mnt/imageinput/software/avs2yuv/avs2yuv.exe $avsscript - > $imgroot/out.yuv
}

if [[ ! -f "$imgroot/out.yuv" ]]
then
    processyuv
fi

if [[ ! -f "$imgroot/content.mp4" ]]
then
avconv -loglevel verbose -y -i $imgroot/out.yuv -threads `nproc` \
    -f mp4 -vcodec libx264 -preset slow -b:v 4000k  -flags +loop \
    -cmp chroma -b:v 1250k -maxrate 4500k -bufsize 4M -bt 256k -refs 1 \
    -bf 3 -coder 1 -me_method umh -me_range 16 -subq 7 -partitions \
    +parti4x4+parti8x8+partp8x8+partb8x8 -g 250 -keyint_min 25 -level 30 \
    -qmin 10 -qmax 51 -qcomp 0.6 -trellis 2 -sc_threshold 40 -i_qfactor 0.71 \
    -acodec aac -strict experimental -b:a 112k -ar 48000 -ac 2 $imgroot/content.mp4
fi
