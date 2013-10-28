#!/bin/bash

let verbose=0
let simulate=0

vOut()
{
    if (($verbose))
    then
        echo $@
    fi
}

doCommand()
{
    vOut $@
    if ((!$simulate ))
    then
        $@
    fi
}

mklinks()
{
	let to=0

	dirs=$(ls -d *PHOTO | sed 's/PHOTO//' | sort -n)

	for dir in $dirs
	do
		numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {} | cut -b5-8 | sort -n)

		for number in $numbers
		do
			filename=SAM_$(printf '%04u' $number).JPG
			ln -s ${dir}PHOTO/$filename SAM_$(printf '%04u' $to).JPG
			((to++))
		done
	done
}

renumber()
{
    let numFiles=$(ls SAM_*.JPG | wc -l)
    let to=${1:-0}
    let from=0
    let count=0

    dest=SAM_`printf "%04u" $to`.JPG
    while [[  -f $dest ]]
    do
        ((to++))
        dest=SAM_`printf "%04u" $to`.JPG
    done
    if (( $to == $numFiles ))
    then
        vOut Nothing to do
        return
    fi
    let from=$to

	while (($count < $numFiles))
    do
        vOut Copy target is $dest 
        source=SAM_`printf "%04u" $from`.JPG
        while [[ ! -f $source ]]
        do
            ((from++))
            source=SAM_`printf "%04u" $from`.JPG
        done

        vOut Copy source is $source 
        while [[ -f $source ]]
        do
            doCommand mv $source $dest
            ((to++))
            ((from++))
            dest=SAM_`printf "%04u" $to`.JPG
            source=SAM_`printf "%04u" $from`.JPG
            ((count++))
        done
    done
}

preview()
{
    if [[ -f preview.mpg ]]
    then
        echo preview.mpg already exists
        return
    fi
    ffmpeg -i SAM_%04d.JPG -b 4000k -vf scale=1024:-1 -vcodec mpeg2video preview.mpg
}


genyuv()
{
    if [[ "$1" == "web" ]]
    then
        let scaleX=300
    fi
    if [[ "$1" == "dvd" ]]
    then
        let scaleX=720
    fi
    crop=($(head -1 crop.cfg)) # left, top, right, bottom 
    # w:h:x:y

    #let width=$(echo "(${crop[2]}-${crop[0]})/2+${crop[0]}" | bc) # (right - left)/2 + left
    let width=$((${crop[2]} - ${crop[0]})) # (right - left)/2 + left
    if [[ "$1" == "hd" ]]
    then
        let scaleX=2048
    fi
    #let height=$(((${crop[3]}-${crop[1]})/2+${crop[1]}))  # (bottom-top)/2 + top
    let height=$((${crop[3]} - ${crop[1]})) # (right - left)/2 + left
    vOut width $width height $height

    scaler=$(echo "scale=2;${width}/${height}" | bc) 
    let scaleY=$(echo "$scaleX/$scaler" | bc)
    rm stream.yuv
    rm stream_${1}.yuv
    doCommand mplayer mf://*.JPG -mf fps=18  -benchmark -nosound -noframedrop -noautosub  -vo yuv4mpeg -vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY
    doCommand mv stream.yuv stream_${1}.yuv
}

mpeg2()
{
    if [[ ! -f stream_${1}.yuv ]]
    then
        genyuv $1
    fi
    bw=4000
    if [ "$1" == 'hd' ]
    then
        bw=8000
    fi
    cat stream_${1}.yuv | yuvfps -r 25:1 -v 1 | mpeg2enc --multi-thread 2 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $(basename `pwd`)_${1}.mpg
}



while getopts "sv" OPT
do
    case $OPT in
        v) let verbose=1 ;;
        s) let simulate=1 ;;
        *) echo What?; exit 1 ;;
    esac
done
shift $((OPTIND-1))
echo $@
case "$1" in 
	renumber) renumber $2 ;;
    preview) preview ;;
    genyuv) genyuv web; genyuv dvd; genyuv hd ;;
    web) mpeg2 web ;;
    dvd) mpeg2 dvd ;;
    hd) mpeg2 hd ;;
    mklinks) mklinks ;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	*) echo What? ;;
esac
