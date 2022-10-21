#!/bin/bash

DEVICE=/dev/video0
SHAREDIR=/home/mattd/share
#VIDEOSIZE=3264x2448
#VIDEOSIZE=640x480
VIDEOSIZE=1280x720

case "$1" in 
    dev) ffmpeg -devices ;;
    caps) ffmpeg -f v4l2 -list_formats all -i ${DEVICE} ;;
    capmjpeg) 
        sudo ffmpeg -f v4l2 -framerate 1 -video_size ${VIDEOSIZE} -i ${DEVICE} -y ${SHAREDIR}/capmjpeg_${VIDEOSIZE}.avi ;;
    copy) sudo ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vcodec copy -y ${SHAREDIR}/rawout_${VIDEOSIZE}.avi ;;
    raw2img) ffmpeg -i ${SHAREDIR}/rawout_${VIDEOSIZE}.avi -vf fps=1 ${SHAREDIR}/frame%d.png ;;
    pipe) sudo ffmpeg -f v4l2 -video_size ${VIDEOSIZE} -i ${DEVICE} -vf fps=1 ${SHAREDIR}/frame%d.png ;;
    stream) ffmpeg -video_size 640x480 -rtbufsize 702000k -framerate 10 -i video="${DEVICE}" -threads 4 -vcodec libh264 -crf 0 -preset ultrafast -f mpegts "udp://pop-os:56666" ;;
    *) echo what?
esac

