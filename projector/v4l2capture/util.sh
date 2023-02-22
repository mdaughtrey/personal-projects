#!/bin/bash

PROJECT=outside
DEVICE=/dev/video0
#VIDEOSIZE=3264x2448
#VIDEOSIZE=640x480
VIDEOSIZE=1280x720

#exec > >(tee -a usb_${OP}_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1
exec > >(tee -a usb_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1

FRAMES=${PWD}/frames/${PROJECT}
if [[ ! -d "${FRAMES}" ]]; then mkdir ${FRAMES}; fi

getdev()
{
    mapfile devices<<<$(v4l2-ctl --list-devices)
    for ((ii=0; ii<${#devices[@]}; ii++)); do
        [[ "${devices[$ii]}" =~ .*SVBONY ]] && echo ${devices[$((ii+1))]} && break
    done
}

clip()
{
    ./autocrop.py --inputdir ${FRAMES} --outputdir ${FRAMES}/cropped --serialize 
}

do8mm()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log --fastforward 9 --res 1 alternate --film 8mm 
}

s8()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log \
        --fastforward 9 --res 1 alternate --film super8 
}

sertest()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log \
        --fastforward 12 --res 1 sertest --film super8 --optocount 6 
}

preview()
{

    	#-quiet -mf fps=10 -benchmark -nosound -noframedrop -noautosub 
    # mplayer \ # -msglevel all=6 -lavdopts threads=`nproc` \
    ls ${FRAMES}/*.png > /tmp/filelist.txt
    mplayer mf://@/tmp/filelist.txt -vf scale=640:480 -vo yuv4mpeg:file=${FRAMES}/preview.mp4
}



case "$1" in 
    avx) ./avx.sh ;;
    dev) ffmpeg -devices ;;
    caps) ffmpeg -f v4l2 -list_formats all -i $(getdev) ;;
    cap1bmp) 
        ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i $(getdev) -vframes 1 -y ${FRAMES}/%08d.bmp ;;
    capmjpeg) 
        ffmpeg -f v4l2 -framerate 1 -video_size ${VIDEOSIZE} -i ${DEVICE}  -vf fps=10 -y ${FRAMES}/capmjpeg_${VIDEOSIZE}.avi ;;
    copy) ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vcodec copy -y ${FRAMES}/rawout_${VIDEOSIZE}.avi ;;
    raw2img) ffmpeg -i ${FRAMES}/rawout_${VIDEOSIZE}.avi -vf fps=10 ${FRAMES}/frame%d.png ;;
    pipe) ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vf fps=1 ${FRAMES}/frame%d.png ;;
    stream) ffmpeg -video_size 640x480 -rtbufsize 702000k -framerate 10 -i video="${DEVICE}" -threads 4 -vcodec libh264 -crf 0 -preset ultrafast -f mpegts "udp://pop-os:56666" ;;
    getdev) getdev ;;
    clip) clip ;;
    8mm) do8mm; preview ;;
    s8) s8; preview ;;
    preview) preview ;;
    sertest) sertest ;;
    *) echo what?
esac

