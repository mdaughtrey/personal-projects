#!/bin/bash
# kill a screen 
# screen -S session -X quit
# 1. Run setres
# 2. startVLC
# 3. 8mm or S8

PORT=/dev/ttyACM0
PROJECT=20240207_test1
FRAMES=${PWD}/frames/
FP=${FRAMES}/${PROJECT}
DEVICE=/dev/video0
#VIDEOSIZE=3264x2448
#VIDEOSIZE=640x480
VIDEOSIZE=1280x720
# Extended Dynamic Range
#EXPOSURES="2500,3000,3500,4000,4500,5000,5500"
EXPOSURES="2000,5000,7000"
EDR="--exposure ${EXPOSURES}"

#exec > >(tee -a usb_${OP}_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1
exec > >(tee -a usb_$(TZ= date +%Y%m%d%H%M%S).log) 2>&1

mkdir -p ${FP}

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
    ./usbcap.py framecap --camindex $(getdev) --framesto ${FP}/capture --frames 10000 --logfile usbcap.log \
        --fastforward 9 --res 0 --film super8 ${EDR}  --startdia 57 --enddia 33
#        --fastforward 9 --res 0 --film super8 --usevlc localhost:4212:abc ${EDR} 
}

sertest()
{
    ./usbcap.py --camindex $(getdev) --framesto ${FRAMES}  --frames 99999 --logfile usbcap.log \
        --fastforward 12 --res 1 sertest --film super8 --optocount 6 
}

p2()
{
    subdir=${1:-capture}
    for exp in ${EXPOSURES//,/ }; do
        ls ${FP}/${subdir}/????????_${exp}.png > /tmp/filelist.txt
        echo Generating ${FP}/{subdir}_${exp}.mp4
        mplayer mf://@/tmp/filelist.txt -vf scale=640:480 -vo yuv4mpeg:file=${FP}/${subdir}_${exp}.mp4
    done
}

preview()
{
    subdir=${1:-car}
    # ls ${FP}/${subdir}/????????.png > /tmp/filelist.txt
    #ls ${FP}/${subdir}/*_5000.png > /tmp/filelist.txt
    #mplayer mf://@/tmp/filelist.txt -vf scale=640:480 -vo yuv4mpeg:file=${FP}/${PROJECT}.mp4
    IFS=, read -ra exs <<<${EXPOSURES}
    for ex in $exs; do
        ffmpeg -f image2 -r 18 -i ${FP}/${subdir}/%08d_${ex}.png -vcodec libx264 -vf scale=640x480 ${FP}/${PROJECT}_${ex}.mp4 
    done
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
    if [[ "$?" ]]; then
        ln -s frames /media/frames 
    fi
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
    ./usbcap.py exptest --camindex $(getdev) --framesto ${FP}/exptest --logfile exptest.log  --nofilm --res 0
}

tonefuse()
{
    ./usbcap.py tonefuse --framesfrom ${FP}/capture --framesto ${FP}
}

oneshot()
{
    ./usbcap.py oneshot --camindex $(getdev) --framesto ${FP} --logfile usbcap.log  --exposure 10000
        
}

setres()
{
    v4l2-ctl --device $(getdev)  --set-fmt-video=width=2592,height=1944
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
    p2) shift; p2 $@ ;;
    sertest) sertest ;;
    getres) getres ;;
    mount) mount ;;
	gcd) getcamdev ;;
    previews) preview capture; preview descratch; preview graded ;;
    all) s8; ./colorgrade.py; descratch; previews ;;
    exptest) exptest ;;
    tonefuse) tonefuse ;;
    oneshot) oneshot ;;
    screen) screen -L ${PORT} 115200 ;;
    setres) setres ;;
    startvlc) screen -dmS vlc vlc --intf qt --extraintf telnet --telnet-password abc ;;
    registration) ./00_registration.py --readfrom ${FP}/capture/'*.png' --writeto ${FP}/capture;;
    car) ./01_crop_and_rotate.py --readfrom ${FP}/capture --writeto ${FP}/car --annotate;;
    *) echo what?
esac

