#!/bin/bash
# kill a screen 
# screen -S session -X quit

PROJECT=outside
FRAMES=${PWD}/frames/
FP=${FRAMES}/${PROJECT}
DEVICE=/dev/video0
#VIDEOSIZE=3264x2448
#VIDEOSIZE=640x480
VIDEOSIZE=1280x720
# Extended Dynamic Range
#EDR="--exposure 700,1400"

#exec > >(tee -a usb_${OP}_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1
exec > >(tee -a usb_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1

for sd in capture graded descratch work; do
    if [[ ! -d "${FP}/${sd}" ]]; then mkdir -p ${FP}/${sd}; fi
done

getcamdev()
{
	DEVID=$(lsusb | grep 'SVBONY SV205' | awk '{print $6}')
    idV=${DEVID%:*}
    idP=${DEVID#*:}
    for path in `find /sys/ -name idVendor | rev | cut -d/ -f 2- | rev`; do
        if grep -q $idV $path/idVendor; then
            if grep -q $idP $path/idProduct; then
                find $path -name 'device' | rev | cut -d / -f 2 | rev
            fi
        fi
    done
}

getdev()
{
    mapfile devices<<<$(v4l2-ctl --list-devices)
    for ((ii=0; ii<${#devices[@]}; ii++)); do
        [[ "${devices[$ii]}" =~ .*SVBONY ]] && echo ${devices[$((ii+1))]} && break
    done
}

#clip()
#{
#    ./autocrop.py --inputdir ${FRAMES} --outputdir ${FRAMES}/cropped --serialize 
#}
#
#do8mm()
#{
#    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log --fastforward 9 --res 1 alternate --film 8mm 
#}

s8()
{
    ./usbcap.py framecap --camindex $(getdev) --framesto ${FP}/capture  --frames 99999 --logfile usbcap.log \
        --fastforward 9 --res 1 --film super8 ${EDR} 
}

sertest()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log \
        --fastforward 12 --res 1 sertest --film super8 --optocount 6 
}

preview()
{
    subdir=${1:-capture}
    ls ${FP}/${subdir}/????????.png > /tmp/filelist.txt
    mplayer mf://@/tmp/filelist.txt -vf scale=640:480 -vo yuv4mpeg:file=${FP}/${subdir}.mp4
}

getres()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FP}  --frames 99999 --logfile usbcap.log --fastforward 9 --res 1 getres --film 8mm 
}

mount()
{
#    sudo mount.cifs //NAS-36-FE-22/imageinput /media/nas -o user=nobody,password=nobody,rw,file_mode=077r_mode=07777
#    ln -s frames /media/nas/frames 
    sudo mount /dev/sda1 /media/frames -o user=nobody,password=nobody,rw,file_mode=077r_mode=07777
    ln -s frames /media/frames 
}

descratch()
{
    DOCKERNAME=avxsynth
    SCRIPT=${FP}/avx.avs
    mkdir -p ${FP}/descratch
#    OUT="${FRAMES}/descratch/%08d.png"
    let NUMFRAMES=$(ls ${FP}/graded/????????.png | wc -l)

(cat <<AVS
LoadPlugin("/AviSynthPlus/avisynth-build/plugins/ImageSeq/libimageseq.so")
LoadPlugin("/RemoveDirt/build/RemoveDirt/libremovedirt.so")
LoadPlugin("/mvtools/build/Sources/libmvtools2.so")
LoadPlugin("/RgTools/build/RgTools/librgtools.so")
LoadPlugin("/mvtools/build/DePanEstimate/libdepanestimate.so")
LoadPlugin("/mvtools/build/DePan/libdepan.so")
LoadPlugin("/mvtools/build/Sources/libmvtools2.so")
function RemoveDirt(clip input, bool "_grey", int "repmode") 
{
    _grey=default(_grey, false)
    repmode=default(repmode, 16)
    clmode=17
    clensed=Clense(input, grey=_grey, cache=4)
    sbegin = ForwardClense(input, grey=_grey, cache=-1)
    send = BackwardClense(input, grey=_grey, cache=-1)
    alt=Repair(SCSelect(input, sbegin, send, clensed, debug=true), input, mode=repmode, modeU = _grey ? -1 : repmode ) 
    restore=Repair(clensed, input, mode=repmode, modeU = _grey ? -1 : repmode)
    corrected=RestoreMotionBlocks(clensed, restore, neighbour=input, alternative=alt, gmthreshold=70, dist=1, dmode=2, debug=false, noise=10, noisy=12, grey=_grey)
    return RemoveGrain(corrected, mode=clmode, modeU = _grey ? -1 : clmode )
}	
RemoveDirt(ImageReader("/frames/capture/%08d.png", fps=18.0, end=${NUMFRAMES}).converttoYV12())
AVS
)  > ${SCRIPT}

#    for ff in ${FRAMES}/*.png; do convert $ff -resize 2160x1458\! $ff;  done
    docker run --rm -h ${DOCKERNAME} --name ${DOCKERNAME} -t -v ${FP}/:/frames --entrypoint /ffmpeg/ffmpeg  ${DOCKERNAME}:latest -i /frames/avx.avs -y /frames/descratch/%08d.png
}

exptest()
{
    if [[ ! -d "${FP}/exptest" ]]; then mkdir ${FP}/exptest; fi
    ./usbcap.py exptest --camindex $(getdev) --framesto ${FP}/exptest --logfile exptest.log 
}

tonefuse()
{
    ./usbcap.py tonefuse --framesfrom ${FP}/capture --framesto ${FP}
}

oneshot()
{
    ./usbcap.py oneshot --camindex $(getdev) --framesto ${FP}/capture --logfile usbcap.log 
        
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
    descratch) descratch ;;
    8mm) do8mm; preview ;;
    s8) s8 ;;
    preview) shift; preview $@ ;;
    sertest) sertest ;;
    getres) getres ;;
    mount) mount ;;
	gcd) getcamdev ;;
    previews) preview capture; preview descratch; preview graded ;;
    all) s8; ./colorgrade.py; descratch; previews ;;
    exptest) exptest ;;
    tonefuse) tonefuse ;;
    oneshoe) oneshot ;;
    *) echo what?
esac

