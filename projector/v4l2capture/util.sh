#!/bin/bash

DEVICE=/dev/video0
SHAREDIR=/home/mattd/share
#VIDEOSIZE=3264x2448
#VIDEOSIZE=640x480
VIDEOSIZE=1280x720

getdev()
{
    mapfile devices<<<$(v4l2-ctl --list-devices)
    for ((ii=0; ii<${#devices[@]}; ii++)); do
        [[ "${devices[$ii]}" =~ .*SVBONY ]] && echo ${devices[$((ii+1))]} && break
    done
}

clip()
{
    ./autocrop.py --inputdir ${SHAREDIR} --outputdir ${SHAREDIR}/cropped --serialize 
}

usbcap()
{
    rm usbcap.log ~/share/*.bmp
    #./usbcap.py --camindex $(getdev) --framesto ${SHAREDIR}  --frames 100 --logfile usbcap.log --fastforward 5 --res 1 alternate
    ./usbcap.py --camindex $(getdev) --framesto ${SHAREDIR}  --frames 1500 --logfile usbcap.log --fastforward 5 --res 1 framecap
#    ./usbcap.py --camindex $(getdev) --framesto ${SHAREDIR}/opto4  --frames 200 --optocount 4 --logfile opto4.log
#    ./usbcap.py --camindex $(getdev) --framesto ${SHAREDIR}/opto8  --frames 200 --optocount 8 --logfile opto8.log
#    ./usbcap.py --camindex $(getdev) --framesto ${SHAREDIR}/opto4_pcd_2  --frames 200 --optocount 4 --logfile opto4_pcd_2.log --postcropdelay 2
}

tovideo()
{

    ls ${SHAREDIR}/*.bmp > /tmp/filelist.txt
    mplayer -msglevel all=6 -lavdopts threads=`nproc` \
	    mf://@/tmp/filelist.txt \
    	-quiet -mf fps=30 -benchmark -nosound -noframedrop -noautosub \
        -vo yuv4mpeg:file=${SHAREDIR}/vdeo.mp4
}



case "$1" in 
    dev) ffmpeg -devices ;;
    caps) ffmpeg -f v4l2 -list_formats all -i $(getdev) ;;
    cap1bmp) 
        ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i $(getdev) -vframes 1 -y ${SHAREDIR}/%08d.bmp ;;
    capmjpeg) 
        ffmpeg -f v4l2 -framerate 1 -video_size ${VIDEOSIZE} -i ${DEVICE}  -vf fps=10 -y ${SHAREDIR}/capmjpeg_${VIDEOSIZE}.avi ;;
    copy) ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vcodec copy -y ${SHAREDIR}/rawout_${VIDEOSIZE}.avi ;;
    raw2img) ffmpeg -i ${SHAREDIR}/rawout_${VIDEOSIZE}.avi -vf fps=10 ${SHAREDIR}/frame%d.png ;;
    pipe) ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vf fps=1 ${SHAREDIR}/frame%d.png ;;
    stream) ffmpeg -video_size 640x480 -rtbufsize 702000k -framerate 10 -i video="${DEVICE}" -threads 4 -vcodec libh264 -crf 0 -preset ultrafast -f mpegts "udp://pop-os:56666" ;;
    getdev) getdev ;;
    clip) clip ;;
    usbcap) usbcap ;;
    tovideo) tovideo ;;
    *) echo what?
esac

