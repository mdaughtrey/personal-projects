#!/bin/bash

#set +o noexec

#PROJECT=hd0
PROJECT=opto2
#TYPE=super8
#WORKING=/media/sf_vproj/scans/
WORKING=/working/scans
#IMPORT=/p/vproj/scans
#WORKING=/media/sf_vproj/scans/
WP="${WORKING}/${PROJECT}"
if [[ ! -d "${WP}" ]]; then
    mkdir -p ${WP}
fi

#if [[ ! -f "/tmp/filelist.txt" ]]; then
#	ls ${WP}/png_cropped/*.png | sed -n '1~6!p' > /tmp/filelist.txt
#fi

RES=hd
while getopts 'p:r:' arg; do
    case "${arg}" in
    p) PROJECT=${OPTARG} ;;
    r) RES=${OPTARG} ;;
    esac
done

shift $((OPTIND-1))

#while getopts 'dw:p:' arg; do
#    case ${arg} in
#        d) RES=draft ;;
#        w) W=${ARG} ;;
#        p) P=${ARG} ;;
#        *) echo What?; exit 1;;
#    esac
#done
#WP=${W}/${P}
#echo Working project is ${WP}, ${RES}

# Copy RGB files from camera to local dir
import()
{
    let dirnum=0
    while [[ /bin/true ]]; do
        targetdir=${WP}"/"$(printf '%03d' ${dirnum})
    	echo Copying to ${targetdir}
        sourcefile=$(printf '00%d????.rgb' ${dirnum})
        mkdir -p  ${targetdir}
        rsync -v -u mattd@hqcam:/mnt/exthd/${PROJECT}/${sourcefile} ${targetdir}
        if [[ -z "$(ls -A ${targetdir})" ]]; then
            rm -rf ${targetdir}
            echo Import complete
            exit 0
        fi
        ((dirnum+=1))
    done
    #echo scp mattd@hqcam:/mnt/exthd/${PROJECT}/${sourcefile}*.rgb ${targetdir}
}

yuv()
{
#    ls ${WP}/png_cropped/??????a.png > /tmp/filelist.txt
#    mplayer -msglevel all=6 -lavdopts threads=`nproc` \
#        mf://@/tmp/filelist.txt \
#    	-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub \
#        -vo yuv4mpeg:file=${WP}/video0.yuv


            ffmpeg -loglevel info -i ${WP}/video0.yuv -vcodec rawvideo -y ${WP}/video0.avi


#	ffmpeg -framerate 15 -pattern_type glob -i ${WP}/png_cropped/'??????a.png' -c:v libx264 -pix_fmt yuv420p ${WP}/video0.avi
#    mkdir ${WP}/deshake
#	#ls ${WP}/png_cropped/*.png | sed -n '1~6!p' | sed "s/.*/file '\0'/" > /tmp/filelist.txt
#	ls ${WP}/png_cropped/*.png | sed -n '1~6!p' > /tmp/filelist.txt
#	video0="${WP}/video0.avi"
#	if [[ ! -f "$video0" ]]; then
#        mplayer -msglevel all=6 -lavdopts threads=`nproc` \
#            mf://@/tmp/filelist.txt -quiet -mf fps=15 $video0
##            -quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub -vo avi:file=$video0
#	fi
#    ffmpeg -loglevel verbose -i $video0 -vf deshake=x=384:y=579:w=316:h=721:ry=64 ${WP}/deshake/%06d.png 
}

deshake()
{
    mkdir ${WP}/deshake
	#ls ${WP}/png_cropped/*.png | sed -n '1~6!p' | sed "s/.*/file '\0'/" > /tmp/filelist.txt
	ls ${WP}/png_cropped/*.png | sed -n '1~6!p' > /tmp/filelist.txt
	video0="${WP}/video0.yuv"
	if [[ ! -f "$video0" ]]; then
        mplayer -msglevel all=6 -lavdopts threads=`nproc` \
            mf://@/tmp/filelist.txt \
            -quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub \
            -vo yuv4mpeg:file=$video0
	fi
    ffmpeg -loglevel verbose -i $video0 -vf deshake=x=384:y=579:w=316:h=721:ry=64 ${WP}/deshake/%06d.png 
}

vidstab()
{
	vslist="/tmp/vidstablist.txt"
	ls ${WP}/png_cropped/*.png | sed -n '1~6!p' > ${vslist}
	video0="${WP}/video0.yuv"
	transforms="${WP}/transformx.trf"
	vidstab="${WP}/vidstab.avi"
	if [[ ! -f "$video0" ]]; then
		mplayer -msglevel all=6 -lavdopts threads=`nproc` \
		    mf://@${vslist} \
		    -quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub \
		    -vo yuv4mpeg:file=$video0
	fi

	if [[ ! -f "$transforms" ]]; then 
		echo Building stab transform
		ffmpeg -loglevel verbose -y -i $video0 \
			-vf vidstabdetect=stepsize=32:shakiness=10:accuracy=10:result=${transforms} -f null -
	fi
	if [[ ! -f "${vidstab}" ]]; then
	    	ffmpeg -loglevel verbose -y -i $video0 \
		-vf "vidstabtransform=input=${transforms}:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4,nlmeans" \
-c:v huffyuv -pix_fmt yuv420p $vidstab
#crop=2122:1522:664:298" -c:v huffyuv -pix_fmt yuv420p $vidstab
	#	-pix_fmt yuv420p  ${vidstab}
	fi
#crop=2122:1522:664:298" ${WP}/vidstab/%06d.png
    #ffmpeg -loglevel verbose -y -i $video0 -vf vidstabtransform=input=${transforms}:zoom=0:smoothing=10 ${WP}/vidstab/%06d.png
}

flowframes()
{
    DIR=/c/vproj/scans/${PROJECT}/split-video/
#    RIFE=/mnt/c/Users/matt/AppData/Local/Flowframes/FlowframesData/pkgs/rife-cuda/
    pushd ${RIFE}

    for YUV in ${DIR}/*.yuv; do
        FILE=$(basename $YUV)
        FRAMES=${FILE}-frames
        mkdir -p ${DIR}/${FRAMES}
        ffmpeg -i ${YUV} ${DIR}/${FRAMES}/%3d.png
#        python3 -u rife.py --input ${DIR}/${FRAMES} --output ${FRAMES}/interp --model RIFE31 --exp 2 --UHD --wthreads 8 --rbuffer 200
#        exit 0
    done
    popd

#/mnt/c/Users/matt/AppData/Local/Flowframes/FlowframesData/pkgs/rife-cuda
}


doscenedetect()
{
	scenedetect -m 3s -i ${WP}/vidstab.avi detect-content -t 40 split-video -o ${WP}/split-video \
		--filename '$VIDEO_NAME-Scene-$SCENE_NUMBER.yuv' 
#	mkdir ${WP}/split-video
#/usr/local/lib/python3.9/dist-packages/scenedetect-0.5.5-py3.9.egg/scenedetect/__main__.py
	#scenedetect -i ${WP}/video0.yuv -s stats.csv detect-content list-scenes 
	#scenedetect -i ${WP}/vidstab/%06d.png -s stats.csv --min-scene-len 3s --drop-short-scenes -o ${WP}/split-video detect-content 
	#scenedetect -i ${WP}/vidstab/%06d.png --min-scene-len 3s --drop-short-scenes -o ${WP}/split-video detect-content 

# 	scenedetect -i ${WP}/video0.yuv -o ${WP}/split-video save-images detect-content
}

stitch()
{
#	    ls ${WP}/vidstab/*.png | sed "s/.*/file '\0'/" >> /tmp/filelist.txtk
	rm -f /tmp/filelist.txt
	DIRS=$(ls -d ${WP}/split-video/*-interp | sort)
	for CUR in $DIRS; do
		#ls $CUR/*.png | sort | sed "s/.*/file '\0'/" >> /tmp/filelist.txt
		ls $CUR/*.png | sort  >> /tmp/filelist.txt
	done
        #ffmpeg -y -f concat -safe 0 -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p ${WP}/${PROJECT}_final.mp4
#        ffmpeg -y -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p ${WP}/${PROJECT}_final.mp4

    	mplayer -msglevel all=6 -lavdopts threads=`nproc` \
	        mf://@/tmp/filelist.txt \
    		-quiet -mf fps=30 -benchmark -nosound -noframedrop -noautosub \
	        -vo yuv4mpeg:file=${WP}/${PROJECT}_final.mp4

#	    ffmpeg -loglevel verbose -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;
}

# Generate draft mp4 files to check cropping etc
peek()
{
    case "${RES}" in
        draft) scale=640; croptype=showcrop ;;
        hd) scale=1080; croptype=cropped ;;
        *) echo Unknown res [${RES}]; exit 1;;
    esac
    ls ${WP}/png_${croptype}_${RES}/??????a.png  > /tmp/filelist.txt

	mplayer -msglevel all=6 -lavdopts threads=`nproc` \
		mf://@/tmp/filelist.txt \
		-quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub \
       	    -vo yuv4mpeg:file=${WP}/${RES}.yuv
       	    #-vo yuv4mpeg:file=${WP}/peek.yuv
    ffmpeg -i ${WP}/${RES}.yuv -vf scale=${scale}:-2 ${WP}/${RES}.mp4
}

#build()
#{
#	ls ${WP}/title/title[0-9]?????.png | sed "s/.*/file '\0'/" > /tmp/filelist.txt
#
#    if [[ "$1" == "vidstab" ]]; then
#	    ls ${WP}/vidstab/*.png | sed "s/.*/file '\0'/" >> /tmp/filelist.txtk
#    fi
    #ffmpeg -y -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${WP}/${PROJECT}_vidstab.mp4
#    ffmpeg -y -i ${WP}/title/title[0-9]*.png -i ${WP}/vidstab/*.png -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${WP}/${PROJECT}_vidstab.mp4
# "crop w:h:x:y"
#    ffmpeg -y -pattern_type glob -threads `nproc`  -thread_queue_size 512 -i ${WP}'/interpolated/vidstab-2x-RIFE/%08d.png' -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage -vv "crop=2122:1522:664:298" :${WP}/${PROJECT}_vidstab.mp4
#}
#
#			 $video1
#	fi
# -------------------------------------------------------------------------
#                        DESHAKE

    #ffmpeg -i $video0 -vf deshake,yadif=0:01:0,nlmeans -filter:v hqdn3d=4.0:3.0:6.0:4.5 ${WP}/stabilized/%06d.png



#	        -pix_fmt yuv420p ${WORKING}/${PROJECT}/png_interplated/%06d.png
#    return
#	ffmpeg -y -i /tmp/filelist.txt -vf "vidstabtransform=input=${transforms}:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4, yadif=0:01:0,nlmeans" \
#        -pix_fmt yuv420p ${WORKING}/${PROJECT}/png_interplated/%06d.png

#    -vcodec libx264 -tune film -acodec copy -preset slow ${WORKING}/${PROJECT}/${PROJECT}_stab_denoise.mp4


#	ffmpeg -y -i /tmp/filelist.txt -vf "vidstabtransform=input=${transforms}:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4, yadif=0:01:0,nlmeans" -vcodec libx264 -tune film -acodec copy -preset slow ${WORKING}/${PROJECT}/${PROJECT}_stab_denoise.mp4


#	ffmpeg -r 18 -i ${WORKING}/${PROJECT}/png/%06d.png -vf minterpolate=fps=30:scd=none:mi_mode=blend -pix_fmt yuv420p \
#		${WORKING}/${PROJECT}/png_interpolated/%06d.png

#	if [[ ! -f "${transforms}" ]]; then
#	    ffmpeg -i ${WORKING}/${PROJECT}/rife_25fps_hd.mp4 -vf vidstabdetect=stepsize=32:shakiness=10:accuracy=10:result=${transforms} -f null -
#	fi
#	    ffmpeg -loglevel verbose -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;

# Workflow
# 1. Import
# pngcrop ??? -> cropped_${RES}
# Peek to generate ${RES}.yuv, ${RES}.mp4
# 2. pngcrop -> png_cropped 150
# 3. title ->  -> png_cropped 0-149
# 4. vidstab
# 5. scenedetect
# 6. flowframes input = vidstab
# 7: flowframes ps1
# 8: stitch
processavi()
{
    MYDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    SOFTWARE=/media/sf_vproj/scans/software/
    let count=0
    for avifile in ${WP}/video?.avi; do
        avsscript=/tmp/interpolate.avs
        outfile=${WP}/vsynth_${count}.avi
        if [[ ! -f "$outfile" ]]; then 
            echo "film=\"${avifile}\"" > $avsscript
            echo 'result="result1"' >> $avsscript
            cat ../hq_interpolate2.avs >> $avsscript
            WINEDLLPATH=~/.wine/drive_c/windows/system32 
           export WINEDEBUG=trace+all
            wine ${SOFTWARE}/avs2yuv/avs2yuv.exe -v $avsscript - > ${outfile}  2> gencontenthq.log
        fi
        ((count++))
    done
}

pngcrop()
{
    if [[ "draft" == "${RES}" ]]; then
        ./hqtest.py --project ${PROJECT} --root ${WORKING} --format png --overwrite --res draft --showcrop --serialize
    else
        ./hqtest.py --project ${PROJECT} --root ${WORKING} --format png --overwrite --res hd 
    fi
}

case "$1" in 
    #draft) export RES=draft; import; pngcrop; peek ;;
    draft) export RES=draft; pngcrop; peek ;;
    title) ../gentitlehq.sh -r ${WORKING} -p ${PROJECT} ;; # -u ac ;;
#    gen) use=${2:-ac}
#        bash -x ../gencontenthq.sh -h -r ${WORKING} -p ${PROJECT} -u ${use} ;;
    import) import ;;
    pngcrop) pngcrop ;;
#    pngcrop) ./hqtest.py --project ${PROJECT} --root ${WORKING} --format png  --serialize --draft --onefile 000682a.rgb --overwrite ;;
    deshake) deshake > deshake.log 2>&1 ;;
    vidstab) vidstab ;; # > vidstab.log 2>&1 ;;
    scenedetect) doscenedetect ;;
    flowframes) flowframes ;;
    build) build $2 ;;
    stitch) stitch ;;
    peek) peek ;;
    peekcb) peek cb ;;
    graph) ./graph.py ${WP} ;;
    yuv) yuv ;;
    processavi) processavi ;;
#    png2mp4) ffmpeg -i ${WP}/png_stabilized-2x-RIFE/%08d.png -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${WP}/final.mp4 ;;
    *) echo What?
esac
