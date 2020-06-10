#!/bin/bash

set -o xtrace

#sequence='747 1114'
sequence='777 877'
filedir=./
width=320
#width=640
#height=480
height=240
#vbitrate=8000
vbitrate=2000
scale="${width}:${height}"
fps=16


function createRawMovie
{
    printf "$filedir/IMG_%04u.JPG\n" `seq $sequence` > filelist.txt
# Create raw movie
    if [[ ! -f output_${fps}.avi ]]; then
        /usr/bin/mencoder mf://@filelist.txt -mf fps=$fps -vf scale=$scale -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=${vbitrate}:vpass=1 -o output_${fps}fps.avi
        /usr/bin/mencoder mf://@filelist.txt -mf fps=$fps -vf scale=$scale -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=${vbitrate}:vpass=2 -o output_${fps}fps.avi
    fi
}

function createPcImages
{
# Generate perspective corrected and cropped images
    printf "$filedir/IMG_%04u\n" `seq $sequence` > pc_filelist.txt

    for file in `cat pc_filelist.txt`; do
        if [[ ! -f ${file}_pc.jpg ]]; then
            convert ${file}.JPG -distort Perspective '188,436 188,436   3096,248 3096,436   188,2848 188,2848   3096,2668 3096,2848' -crop '2908x2412+188+436' +repage -scale 320 ${file}_pc.jpg
        fi
        if [[ ! -f {$file}_smooth.jpg ]]; then
            /usr/share/doc/cimg-dev/examples/pde_TschumperleDeriche2d -i ${file}_pc.jpg -iter 100 -o ${file}_smooth.jpg
        fi
     done
}

function createPcMovie
{
# Create perspective corrected and cropped movie
    if [[ ! -f output_${fps}_pc.avi ]]; then
        printf "$filedir/IMG_%04u_smooth.jpg\n" `seq $sequence` > pc_filelist.txt
        /usr/bin/mencoder mf://@pc_filelist.txt -mf fps=$fps -vf scale=$scale -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=${vbitrate}:vpass=1 -o output_${fps}fps_pc.avi
        /usr/bin/mencoder mf://@pc_filelist.txt -mf fps=$fps -vf scale=$scale -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=${vbitrate}:vpass=2 -o output_${fps}fps_pc.avi
    fi
}

case "$1" in 
    cpi) createPcImages; exit ;;
    cpm) createPcMovie; exit ;;
esac


createRawMovie
createPcImages
createPcMovie


rm filelist.txt
rm pc_filelist.txt


