#!/bin/bash

let verbose=0
let simulate=0
let numTitleFrames=90

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


#    while [[  -f $dest ]]
#    do
#        ((to++))
#        dest=SAM_`printf "%04u" $to`.JPG
#    done
#    if (( $to == $numFiles ))
#    then
#        vOut Nothing to do
#        return
#    fi
#    let from=$to

        source=SAM_`printf "%04u" $from`.JPG
        while [[ ! -f $source ]]
        do
            ((from++))
            source=SAM_`printf "%04u" $from`.JPG
        done

    if (($from == $to))
    then
        echo source = dest
        return
     fi

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

let scaleX=0
let scaleY=0
let width=0
let height=0 
let bw=4000

scaler()
{
    if [[ "$1" == "web" ]]
    then
        let scaleX=300
    fi
    if [[ "$1" == "dvd" ]]
    then
        let scaleX=720
    fi
    if [ "$1" == 'hd' ]
    then
        let bw=8000
        let scaleX=2048
    fi
    crop=($(head -1 crop.cfg)) # left, top, right, bottom 

    let width=$((${crop[2]} - ${crop[0]})) # (right - left)/2 + left
    let height=$((${crop[3]} - ${crop[1]})) # (right - left)/2 + left
    scaler=$(echo "scale=2;${width}/${height}" | bc) 
    let scaleY=$(echo "$scaleX/$scaler" | bc)

    vOut Scaler width $width height $height scaleX $scaleX scaleY $scaleY bw $bw
}

genyuv()
{
    scaler $1
    rm stream.yuv
    rm stream_${0}.yuv
    doCommand mplayer mf://*.JPG -mf fps=18  -benchmark -nosound -noframedrop -noautosub  -vo yuv4mpeg -vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY
    doCommand mv stream.yuv stream_${1}.yuv
}

mpeg2()
{
    if [[ ! -f stream_${1}.yuv ]]
    then
        genyuv $1
    fi
    if [[ ! -f title.yuv ]]
    then
	echo Need title.yuv
	exit 1
    fi
	cat stream_${1}.yuv | sed '1d' > stream2_${1}.yuv
    cat title.yuv stream2_${1}.yuv  | yuvfps -r 25:1 -v 1 | mpeg2enc --multi-thread 2 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $(basename `pwd`)_${1}.mpg
}

gentitle()
{
	type=${1:-"dvd"}
set -o xtrace
	scaler $1
	firstfile=${2:-"SAM_0000.JPG"}
	let row=300
	let rowincrement=300
	let pointsize=324
	let shrinkby=120
	if [[ "dvd" == "$type" ]]
	then
		let row=100
		let rowincrement=100
		let pointsize=108
		let shrinkby=40
	fi

	convert $firstfile -crop ${width}x${height}+${crop[0]}+${crop[1]} -scale ${scaleX}x${scaleY} underlay.jpg

	for font in "URW-Chancery-Medium-Italic"
	#for font in `cat fonts.txt`
	do
			cmd="convert -size ${scaleX}x${scaleY} -background none xc:transparent -font $font"
			cmd=$cmd$(cat title.txt | while read line
			do
				if [[ "=" == "$line" ]]
				then
					((pointsize-=$shrinkby))
					((rowincrement-=$shrinkby))
					continue
				fi
				echo " -pointsize $pointsize"
				echo ' -fill black -stroke yellow -strokewidth 2' | tr -d '\n'
				echo " -draw " '"' "text 40,$row" "'""$line""'"'"' | tr -d '\n'
				((row+=rowincrement))
			done
			)" title_${font}.png"
			echo $cmd | sh
			composite -gravity center title_${font}.png underlay.jpg title000.jpg
			rm title_${font}.png
			rm underlay.jpg
#			for ff in `seq 0 $numTitleFrames`
#			do
#				cp title000.jpg title`printf "%04u" $ff`.jpg
#			done
	done

    	doCommand mplayer mf://title000.jpg -mf fps=.2  -benchmark -nosound -noframedrop -noautosub  -vo yuv4mpeg #-vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY
	mv stream.yuv title.yuv
    	cat title.yuv | yuvfps -r 25:1 -v 1 | mpeg2enc --multi-thread 2 -f 0 -a 1 -b $bw -V 3000 -q 1 -o title.mpg
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
#echo $@
case "$1" in 
	title) gentitle $2 ;;
	renumber) renumber $2 ;;
    preview) preview ;;
    genyuv) genyuv web; genyuv dvd; genyuv hd ;;
    web) scaler web; gentitle web; mpeg2 web ;;
    dvd) scaler dvd; gentitle dvd; mpeg2 dvd ;;
    hd) mpeg2 hd ;;
    mklinks) mklinks ;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	*) echo What? ;;
esac
