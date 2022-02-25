#!/bin/bash

#set +o noexec

PROJECT=mechtest12
#TYPE=super8
#ROOTOFALL=/media/sf_vproj/scans/
#ROOTOFALL=/media/currentscan/scans/
#IMPORT=/p/vproj/scans
ROOTOFALL=/media/sf_vproj/scans/
mkdir -p $ROOTOFALL/$PROJECT
RP="${ROOTOFALL}/${PROJECT}"

#if [[ ! -f "/tmp/filelist.txt" ]]; then
#	ls ${RP}/png_cropped/*.png | sed -n '1~6!p' > /tmp/filelist.txt
#fi

import()
{
    let dirnum=0
    while [[ /bin/true ]]; do
        targetdir=${ROOTOFALL}${PROJECT}"/"$(printf '%03d' ${dirnum})
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

deshake()
{
    mkdir ${RP}/deshake
	#ls ${RP}/png_cropped/*.png | sed -n '1~6!p' | sed "s/.*/file '\0'/" > /tmp/filelist.txt
	ls ${RP}/png_cropped/*.png | sed -n '1~6!p' > /tmp/filelist.txt
	video0="${RP}/video0.yuv"
	if [[ ! -f "$video0" ]]; then
        mplayer -msglevel all=6 -lavdopts threads=`nproc` \
            mf://@/tmp/filelist.txt \
            -quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub \
            -vo yuv4mpeg:file=$video0
	fi
    ffmpeg -loglevel verbose -i $video0 -vf deshake=x=384:y=579:w=316:h=721:ry=64 ${RP}/deshake/%06d.png 
}

vidstab()
{
	vslist="/tmp/vidstablist.txt"
	ls ${RP}/png_cropped/*.png | sed -n '1~6!p' > ${vslist}
	video0="${RP}/video0.yuv"
	transforms="${RP}/transformx.trf"
	vidstab="${RP}/vidstab.avi"
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
#crop=2122:1522:664:298" ${RP}/vidstab/%06d.png
    #ffmpeg -loglevel verbose -y -i $video0 -vf vidstabtransform=input=${transforms}:zoom=0:smoothing=10 ${RP}/vidstab/%06d.png
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
	scenedetect -m 3s -i ${RP}/vidstab.avi detect-content -t 40 split-video -o ${RP}/split-video \
		--filename '$VIDEO_NAME-Scene-$SCENE_NUMBER.yuv' 
#	mkdir ${RP}/split-video
#/usr/local/lib/python3.9/dist-packages/scenedetect-0.5.5-py3.9.egg/scenedetect/__main__.py
	#scenedetect -i ${RP}/video0.yuv -s stats.csv detect-content list-scenes 
	#scenedetect -i ${RP}/vidstab/%06d.png -s stats.csv --min-scene-len 3s --drop-short-scenes -o ${RP}/split-video detect-content 
	#scenedetect -i ${RP}/vidstab/%06d.png --min-scene-len 3s --drop-short-scenes -o ${RP}/split-video detect-content 

# 	scenedetect -i ${RP}/video0.yuv -o ${RP}/split-video save-images detect-content
}

stitch()
{
#	    ls ${RP}/vidstab/*.png | sed "s/.*/file '\0'/" >> /tmp/filelist.txtk
	rm -f /tmp/filelist.txt
	DIRS=$(ls -d ${RP}/split-video/*-interp | sort)
	for CUR in $DIRS; do
		#ls $CUR/*.png | sort | sed "s/.*/file '\0'/" >> /tmp/filelist.txt
		ls $CUR/*.png | sort  >> /tmp/filelist.txt
	done
        #ffmpeg -y -f concat -safe 0 -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p ${RP}/${PROJECT}_final.mp4
#        ffmpeg -y -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p ${RP}/${PROJECT}_final.mp4

    	mplayer -msglevel all=6 -lavdopts threads=`nproc` \
	        mf://@/tmp/filelist.txt \
    		-quiet -mf fps=30 -benchmark -nosound -noframedrop -noautosub \
	        -vo yuv4mpeg:file=${RP}/${PROJECT}_final.mp4

#	    ffmpeg -loglevel verbose -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;
}


peek()
{
	#ls ${RP}/png_cropped/*.png | sed "s/.*/file '\0'/" > /tmp/filelist.txt
	ls ${RP}/png_cropped/*.png  > /tmp/filelist.txt
    	#ffmpeg -y -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p ${RP}/${PROJECT}_peek.mp4
	if [[ ! -f "${RP}/peek.yuv" ]]; then
		mplayer -msglevel all=6 -lavdopts threads=`nproc` \
		    mf://@/tmp/filelist.txt \
		    -quiet -mf fps=15 -benchmark -nosound -noframedrop -noautosub \
       	     	-vo yuv4mpeg:file=${RP}/peek.yuv
	fi
	ffmpeg -i ${RP}/peek.yuv -vf scale=720:-2 ${RP}/peek.mp4
#	rm ${RP}/peek.yuv
}

#build()
#{
#	ls ${RP}/title/title[0-9]?????.png | sed "s/.*/file '\0'/" > /tmp/filelist.txt
#
#    if [[ "$1" == "vidstab" ]]; then
#	    ls ${RP}/vidstab/*.png | sed "s/.*/file '\0'/" >> /tmp/filelist.txtk
#    fi
    #ffmpeg -y -i /tmp/filelist.txt -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${RP}/${PROJECT}_vidstab.mp4
#    ffmpeg -y -i ${RP}/title/title[0-9]*.png -i ${RP}/vidstab/*.png -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${RP}/${PROJECT}_vidstab.mp4
# "crop w:h:x:y"
#    ffmpeg -y -pattern_type glob -threads `nproc`  -thread_queue_size 512 -i ${RP}'/interpolated/vidstab-2x-RIFE/%08d.png' -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage -vv "crop=2122:1522:664:298" :${RP}/${PROJECT}_vidstab.mp4
#}
#
#			 $video1
#	fi
# -------------------------------------------------------------------------
#                        DESHAKE

    #ffmpeg -i $video0 -vf deshake,yadif=0:01:0,nlmeans -filter:v hqdn3d=4.0:3.0:6.0:4.5 ${RP}/stabilized/%06d.png



#	        -pix_fmt yuv420p ${ROOTOFALL}/${PROJECT}/png_interplated/%06d.png
#    return
#	ffmpeg -y -i /tmp/filelist.txt -vf "vidstabtransform=input=${transforms}:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4, yadif=0:01:0,nlmeans" \
#        -pix_fmt yuv420p ${ROOTOFALL}/${PROJECT}/png_interplated/%06d.png

#    -vcodec libx264 -tune film -acodec copy -preset slow ${ROOTOFALL}/${PROJECT}/${PROJECT}_stab_denoise.mp4


#	ffmpeg -y -i /tmp/filelist.txt -vf "vidstabtransform=input=${transforms}:zoom=0:smoothing=10,unsharp=5:5:0.8:3:3:0.4, yadif=0:01:0,nlmeans" -vcodec libx264 -tune film -acodec copy -preset slow ${ROOTOFALL}/${PROJECT}/${PROJECT}_stab_denoise.mp4


#	ffmpeg -r 18 -i ${ROOTOFALL}/${PROJECT}/png/%06d.png -vf minterpolate=fps=30:scd=none:mi_mode=blend -pix_fmt yuv420p \
#		${ROOTOFALL}/${PROJECT}/png_interpolated/%06d.png

#	if [[ ! -f "${transforms}" ]]; then
#	    ffmpeg -i ${ROOTOFALL}/${PROJECT}/rife_25fps_hd.mp4 -vf vidstabdetect=stepsize=32:shakiness=10:accuracy=10:result=${transforms} -f null -
#	fi
#	    ffmpeg -loglevel verbose -f concat -safe 0 -i /tmp/files.txt -c copy -ac 2 out.mp4 ;;

# Workflow
# 1. Import
# 2. pngcrop -> png_cropped 150
# 3. title ->  -> png_cropped 0-149
# 4. vidstab
# 5. scenedetect
# 6. flowframes input = vidstab
# 7: flowframes ps1
# 8: stitch

case "$1" in 
    title) ../gentitlehq.sh -r ${ROOTOFALL} -p ${PROJECT} ;; # -u ac ;;
#    gen) use=${2:-ac}
#        bash -x ../gencontenthq.sh -h -r ${ROOTOFALL} -p ${PROJECT} -u ${use} ;;
    import) import ;;
    pngcrop) ./hqtest.py --project ${PROJECT} --root ${ROOTOFALL} --format png  --draft --serialize --onefile 000836a.rgb;;
    #pngcrop) ./hqtest.py --project ${PROJECT} --root ${ROOTOFALL} --format png  --draft --serialize --nowrite ;;
#    pngcrop) ./hqtest.py --project ${PROJECT} --root ${ROOTOFALL} --format png  --draft ;;
    deshake) deshake > deshake.log 2>&1 ;;
    vidstab) vidstab ;; # > vidstab.log 2>&1 ;;
    scenedetect) doscenedetect ;;
    flowframes) flowframes ;;
    build) build $2 ;;
    stitch) stitch ;;
    peek) peek ;;
    graph) ./graph.py ${RP} ;;
#    png2mp4) ffmpeg -i ${RP}/png_stabilized-2x-RIFE/%08d.png -vcodec libx264 -r 30 -pix_fmt yuv420p -tune grain,stillimage ${RP}/final.mp4 ;;
    *) echo What?
esac
