#!/bin/bash

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SEM=sem
VRES=576
HRES=720
SOFTWARE=/media/sf_vproj/scans/software/

while getopts "c:p:r:h" OPT
do
    case $OPT in
        p) project=$OPTARG ;;
#        r) fileroot=$OPTARG ;;
#        c) container=$OPTARG ;;
        h) HRES=1920; VRES=1080 ;;
        *) echo What?; exit 1 ;;
    esac
done

export project=$fileroot/$project/
#export projroot=$fileroot/$project
export titleroot=$project/title

#if [[ "" == $project || "" == $fileroot || "" == $container ]]
if [[ "" == $project ]]
then
    echo Project parameter is missing
    exit 1
fi

getImages()
{
    IFS=\|
    select="select container,fused from picdata where fused is not NULL order by container,fused"
    sqlite3 -list ${project}/$(basename $project)db "$select" | uniq | while read container filename
    do
        echo ${project}/${container}/fused/${filename}.jpg
    done
}

genyuvstream()
{
#    $MYDIR/gentitle.sh -p $project -r $fileroot
    ls $project/title/title*.JPG | sort -n > $project/contentlist.txt
#    cp $project/title/titlelist.txt $project/contentlist.txt
    getImages >> $project/contentlist.txt

        #-vf-add rotate=1 
    mplayer -msglevel all=6 -lavdopts threads=`nproc` \
        mf://@$project/contentlist.txt \
        -vf scale=$HRES:$VRES \
    	-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub \
        -vo yuv4mpeg:file=$project/$(basename $project).yuv
}

processavi()
{
    avsscript=/tmp/interpolate.avs
    echo "film=\"${project}/$(basename $project)/content.avi\"" > $avsscript
	#echo -n "film=\"Y:\\\\`basename $PWD`\\\\rawframes.avi\"" > $avsscript
	echo "result=\"result1\" # specify the wanted output here" >> $avsscript
    cat $MYDIR/pp_interpolate2.avs >> $avsscript
    #wine Z:\\mnt\\imageinput\\software\\avs2yuv\\avs2yuv.exe $avsscript - > $project/out.yuv
    WINEDLLPATH=~/.wine/drive_c/windows/system32 
#    export WINEDEBUG=trace+all
    #wine /mnt/imageinput/software/avs2yuv/avs2yuv.exe $avsscript - > $project/out.yuv
    echo wine ${SOFTWARE}/avs2yuv/avs2yuv.exe $avsscript - > $project/$(basename $project).yuv
}

#if [[ ! -f "$project/$(basename $project).yuv" ]]; then processyuv; fi
if [[ ! -f "$project/$(basename $project).yuv" ]]; then genyuvstream; fi
if [[ ! -f "$project/$(basename $project).avi" ]]; then cat $project/$(basename $project).yuv | yuvfps -v 0 -r 18:1 -v 1 | \
            avconv -loglevel info -i - -vcodec rawvideo -y $project/$(basename $project).avi; fi
if [[ -f "$project/$(basename $project).yuv" ]]; then processavi; fi
exit 0


if [[ ! -f "$project/content.mp4" ]]
then
avconv -loglevel verbose -y -i $project/$(basename $project).yuv -threads `nproc` \
    -f mp4 -vcodec libx264 -preset slow -b:v 4000k  -flags +loop \
    -cmp chroma -b:v 1250k -maxrate 4500k -bufsize 4M -bt 256k -refs 1 \
    -bf 3 -coder 1 -me_method umh -me_range 16 -subq 7 -partitions \
    +parti4x4+parti8x8+partp8x8+partb8x8 -g 250 -keyint_min 25 -level 30 \
    -qmin 10 -qmax 51 -qcomp 0.6 -trellis 2 -sc_threshold 40 -i_qfactor 0.71 \
    -acodec aac -strict experimental -b:a 112k -ar 48000 -ac 2 $project/content.mp4 
fi
