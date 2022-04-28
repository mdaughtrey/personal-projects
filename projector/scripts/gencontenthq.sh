#!/bin/bash

MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SEM=sem
VRES=576
HRES=720
SOFTWARE=/media/sf_vproj/scans/software/
#AVCONV=avconv
AVCONV=ffmpeg
use="no"

while getopts "bp:r:hu:" OPT
do
    case $OPT in
        b) backward=1 ;;
        p) project=$OPTARG ;;
        r) fileroot=$OPTARG ;;
        h) HRES=1920; VRES=1080 ;;
        u) use=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

export project=$fileroot/$project/
#export projroot=$fileroot/$project
export titleroot=$project/title

case $use in
    ac) RAWYUV=$project/$(basename $project)_raw
        COOKEDYUV=$project/$(basename $project)
        AVI=$project/$(basename $project)
        MP4=$project/$(basename $project)
        ;;
    pc) RAWYUV=$project/$(basename $project)_pc_raw
        COOKEDYUV=$project/$(basename $project)_pc.yuv
        AVI=$project/$(basename $project)_pc
        MP4=$project/$(basename $project)_pc.mp4
        ;;
    co) RAWYUV=$project/$(basename $project)_co_raw
        COOKEDYUV=$project/$(basename $project)_co.yuv
        AVI=$project/$(basename $project)_co
        MP4=$project/$(basename $project)_co.mp4
        ;;
    *) RAWYUV=$project/$(basename $project)_raw
       COOKEDYUV=$project/$(basename $project).yuv
       AVI=$project/$(basename $project)
       MP4=$project/$(basename $project).mp4
       ;;
esac

#if [[ "" == $project || "" == $fileroot || "" == $container ]]
if [[ "" == $project ]]
then
    echo Project parameter is missing
    exit 1
fi

#getFusedImages()
#{
#    IFS=\|
#    select="select container,fused from picdata where fused is not NULL order by container,fused"
#    sqlite3 -list ${project}/$(basename $project)db "$select" | uniq | while read container filename
#    do
#        echo ${project}/${container}/fused/${filename}.jpg
#    done
#}

#getAutoCroppedImages()
#{
#    IFS=\|
#    select="select container,autocrop from picdata where autocrop is not NULL AND autocroptag='a' order by container,autocrop"
#    sqlite3 -list ${project}/$(basename $project)db "$select" | uniq | while read container filename
#    do
##        echo ${project}/${container}/autocrop/${filename}a.jpg
#    done
#	ls ${project}/???/*.bmp
#}
# 
# getPreCroppedImages()
# {
#     IFS=\|
#     select="select container,precrop from picdata where precrop is not NULL AND precroptag='b' order by container,precrop"
#     sqlite3 -list ${project}/$(basename $project)db "$select" | uniq | while read container filename
#     do
#         echo ${project}/${container}/precrop/${filename}b.jpg
#     done
# }
# 
# getConvertedImages()
# {
#     IFS=\|
#     select="select container,converted from picdata where converted is not NULL AND convertedtag='b' order by container,converted"
#     sqlite3 -list ${project}/$(basename $project)db "$select" | uniq | while read container filename
#     do
#         echo ${project}/${container}/converted/${filename}b.JPG
#     done
# }

genyuvsegs()
{
#    ls $project/title/title*.JPG | sort -n > $project/contentlist.txt
    #ls ${project}/png/*.png | sort -n >> $project/contentlist.txt
    rm ${project}/contentlist*
    ls ${project}/png/*.png | sort -n  | split -l 1000 -d - ${project}/contentlist
#    echo use is $use
#    case "$use" in 
#        ac) getAutoCroppedImages  | sort >> $project/imagelist.txt ;;
#        pc) getPreCroppedImages >> $project/imagelist.txt ;;
#        co) getConvertedImages >> $project/imagelist.txt ;;
#        *) getFusedImages >> $project/imagelist.txt ;;
#    esac
#    if [[ "$backward" == "1" ]]; then
#        tac $project/imagelist.txt >> $project/contentlist.txt
#    else
#        cat $project/imagelist.txt >> $project/contentlist.txt
#    fi

    let count=0
    for ff in ${project}/contentlist*; do
        outfile=${RAWYUV}_${count}.yuv
        if [[ ! -f "$outfile" ]]; then
            mplayer -msglevel all=6 -lavdopts threads=`nproc` \
                mf://@$ff \
                -vf scale=$HRES:$VRES \
                -quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub \
                -vo yuv4mpeg:file=$outfile
        fi
        ((count++))
    done	
}

segstoavi()
{
    let count=0
    for yuv in ${RAWYUV}_*.yuv; do
        outfile=${AVI}_${count}.avi
        if [[ ! -f "${outfile}" ]]; then
            cat ${yuv} | yuvfps -v 0 -r 18:1 -v 1 | \
                ${AVCONV} -loglevel info -i - -vcodec rawvideo -y ${outfile}
        fi
        ((count++))
    done
}

processavi()
{
    let count=0
    for avifile in ${AVI}_*.avi; do
        avsscript=/tmp/interpolate.avs
        outfile=${COOKEDYUV}_vsynth_${count}.avi
        if [[ ! -f "$outfile" ]]; then 
            echo "film=\"${avifile}\"" > $avsscript
            echo 'result="result1"' >> $avsscript
            cat $MYDIR/hq_interpolate2.avs >> $avsscript
            WINEDLLPATH=~/.wine/drive_c/windows/system32 
           export WINEDEBUG=trace+all
            wine ${SOFTWARE}/avs2yuv/avs2yuv.exe -v $avsscript - > ${outfile}  2> gencontenthq.log
        fi
        ((count++))
    done
}

#if [[ ! -f "${RAWYUV}.yuv" ]]; then genyuvstream; fi

#genyuvsegs
#segstoavi
processavi
exit 0

#exit 0
#if [[ ! -f "${AVI}" ]]; then cat ${RAWYUV}.yuv | yuvfps -v 0 -r 18:1 -v 1 | \
#            ${AVCONV} -loglevel info -i - -vcodec rawvideo -y ${AVI}; fi
#if [[ ! -f "${COOKEDYUV}" ]]; then processavi; fi  

if [[ ! -f "${MP4}" ]]
then
    rm /tmp/files.txt
    for f in ${COOKEDYUV}_vsynth_*.avi; do
        outfile="${f/.avi/.mp4}"
        echo file "'"${outfile}"'" >> /tmp/files.txt
	ffmpeg  -loglevel verbose -y -i ${f} -threads `nproc` -vcodec libx264 -ac 2 ${outfile}
	((count++))
    done
#    infiles=""
#    for ff in ${COOKEDYUV}_vsynth_*.avi; do
#        infiles="-i $ff $infiles"
#    done


    #-cmp chroma -b:v 8M -maxrate 16M -bufsize 4M -bt 256k -refs 1 \
#${AVCONV} -loglevel verbose -y -i ${COOKEDYUV} -threads `nproc` \
#${AVCONV} -loglevel verbose -y ${infiles} -threads `nproc` \
#    -f mp4 -vcodec libx264 -preset slow  -flags +loop \
#    -cmp chroma -maxrate 16M -bufsize 4M -bt 256k -refs 1 \
#    -bf 3 -coder 1 -me_method umh -me_range 16 -subq 7 -partitions \
#    +parti4x4+parti8x8+partp8x8+partb8x8 -g 250 -keyint_min 25 -level 30 \
#    -qmin 10 -qmax 51 -qcomp 0.6 -trellis 2 -sc_threshold 40 -i_qfactor 0.71 \
#    -acodec aac -strict experimental -b:a 112k -ar 48000 -ac 2 ${MP4}
#    test) ffmpeg -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;
#${AVCONV} -loglevel verbose -y -i /tmp/files.txt -threads `nproc` -f concat -c copy ${MP4}

ffmpeg -f concat -safe 0 -i /tmp/files.txt -c copy ${MP4}.mp4


fi
