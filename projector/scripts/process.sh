#!/bin/bash

let verbose=0
let simulate=0
let numTitleFrames=90
let fps=30

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

#set -o xtrace
	for ff in *PHOTO/SAM_0*.JPG
        do
             mv $ff $(echo $ff | sed 's/_0/_1/')
	done
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
	let to=0
	for from in $(ls SAM_??????.JPG | cut -c5-10 | sort -n)
	do
		echo mv SAM_${from}.JPG SAM_`printf "%06d" ${to}`.JPG
		((to++))
	done
	return
	for from in $(ls SAM_????.JPG | cut -c5-8 | sort -n)
	do
		echo mv SAM_${from}.JPG SAM_`printf "%04d" ${to}`.JPG
		((to++))
	done
}

renumber2()
{
	let to=0
	for from in $(ls img*.JPG | cut -c4-9 | sort -n)
	do
		echo mv img${from}.JPG SAM_`printf "%04d" ${to}`.JPG
		((to++))
	done
}

preview()
{
    if [[ -f preview.mpg ]]
    then
        echo preview.mpg already exists
        return
    fi
	#set -o xtrace
    #previewfile=$(basename $PWD)_preview.mpg
    previewfile=${PWD//\//_}preview.mpg
	if [[ -f SAM_0000.JPG ]]
	then
    	ffmpeg -i SAM_%04d.JPG -b 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	else
    	ffmpeg -i SAM_%06d.JPG -b 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	fi
    mv $previewfile $HOME/imageinput/previews 
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
#    if [[ ! -f title.mpg ]]
#    then
#	echo Need title.mpg
#	exit 1
#    fi
    dvdfile=${PWD//\//_}dvd.mpg

    cat stream_${1}.yuv  | yuvfps -r ${fps}:1 -v 1 | mpeg2enc --multi-thread 4 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $dvdfile
    mv $dvdfile $HOME/imageinput/dvd
}

gentitle()
{
	if [[ ! -f title.txt ]]
	then
		echo No title.txt
		exit 1
	fi
	type=${1:-"dvd"}
	scaler $type
	firstfile=${2:-"SAM_0000.JPG"}
	let row=300
	let rowincrement=300
	let pointsize=324
	#let shrinkby=120
	if [[ "dvd" == "$type" ]]
	then
		let row=100
		let rowincrement=100
		let pointsize=108
		let shrinkby=40
	fi

	convert $firstfile -crop ${width}x${height}+${crop[0]}+${crop[1]} -scale ${scaleX}x${scaleY} -blur 2x2 -sepia-tone '65%' underlay.jpg

	for font in "URW-Chancery-Medium-Italic"
	#for font in `cat fonts.txt`
	do
			cmd="convert -size ${scaleX}x${scaleY} -background none xc:transparent -font $font "
			cmd=$cmd$(cat title.txt | while read line
			do
				if [[ "=" == "$line" ]]
				then
					#((pointsize-=$shrinkby))
					#((rowincrement-=$shrinkby))
					((pointsize/=2))
					((rowincrement/=2))
					continue
				fi
				echo " -pointsize $pointsize"
				#echo ' -fill blue -stroke black -strokewidth 1' | tr -d '\n'
				echo ' -fill blue' | tr -d '\n'
				echo " -draw " '"' "text 40,$row" "'""$line""'"'"' | tr -d '\n'
				((row+=rowincrement))
			done
			)" title_${font}.png"
			echo $cmd | sh
			composite -gravity center title_${font}.png underlay.jpg title000.jpg
			rm title_${font}.png
			rm underlay.jpg
	done

    	doCommand mplayer mf://title000.jpg -mf fps=.2  -benchmark -nosound -noframedrop -noautosub  -vo yuv4mpeg #-vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY
	mv stream.yuv title.yuv
    	cat title.yuv | yuvfps -r ${fps}:1 -v 1 | mpeg2enc --multi-thread 4 -f 0 -a 1 -b $bw -V 3000 -q 1 -o title.mpg
}

rrenumber()
{
	for ff in ???PHOTO; do cd $ff; process.sh renumber 1000; cd -; done
}

mp42jpg()
{
	if [[ "" == "$1" ]]
	then
		echo No file
		return
	fi
	ffmpeg -i $1 -b 2000 -qscale 1 -qcomp 0 -qblur 0 SAM_%06d.JPG
}

findSyncLight()
{
let step=40
geometry=($(identify -ping $1  | cut -d' ' -f3 | sed 's/x/ /g'))
let imageW=${geometry[0]}
let imageH=${geometry[1]} 

tempImage=tmp.jpg
convert $1 -channel B -separate ${tempImage}

let rectW=20
let rectH=(imageH/3)
let rectX=(imageW-rectW)
let rectY=0



#
# Shift the rectangle from right to left until we hit the light
#
tag=`convert ${tempImage} -crop ${rectW}x${rectH}+${rectX}+${rectY} -resize 1x1 txt: | tail -1`
result=`echo $tag | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
IFS=" "
rgb=(`echo $result`) # R, G, B

while [[ "${rgb[0]}" -lt "5" && "${rgb[1]}" -lt "5" && "${rgb[2]}" -lt "5" ]]
do
	((rectX-=step))
	tag=`convert ${tempImage} -crop ${rectW}x${rectH}+${rectX}+${rectY} -resize 1x1 txt: | tail -1`
	result=`echo $tag | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
	IFS=" "
	rgb=(`echo $result`) # R, G, B
done

#
# Binary search to reduce the rectangle to um... highlight the synch light
#
while ((rectH > step))
do
	((rectH/=2))
	text1=`convert ${tempImage} -crop ${rectW}x${rectH}+${rectX}+${rectY} -resize 1x1 txt: | tail -1`
	result1=`echo $text1 | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
	IFS=" "
	rgb1=(`echo $result1`) # R, G, B

	((rectY+=rectH))
	text2=`convert ${tempImage} -crop ${rectW}x${rectH}+${rectX}+${rectY} -resize 1x1 txt: | tail -1`
	result2=`echo $text2 | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
	IFS=" "
	rgb2=(`echo $result2`) # R, G, B

	test1=(${rgb1[0]}+${rgb1[1]}+${rgb1[2]})
	test2=(${rgb2[0]}+${rgb2[1]}+${rgb2[2]})

	if ((test1>test2))
	then
		((rectY-=rectH))
	fi
done

#
# Slide the rectangle left to get the brightest reading
#
let offset=0
let ii=0
let maxreading=0
let reading=0

while ((ii<step))
do
	let xx=(rectX-ii)
	text=`convert ${tempImage} -crop ${rectW}x${rectH}+${xx}+${rectY} -resize 1x1 txt: | tail -1`
	result=`echo $text | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
	IFS=" "
	rgb=(`echo $result`) # R, G, B
	let reading=(${rgb[0]}+${rgb[1]}+${rgb[2]})
	((ii++))
	if ((reading>maxreading))
	then
	#	echo reading $reading maxreading $maxreading ii $ii offset $offset
		let maxreading=reading
		let offset=ii
	fi
done
((rectX-=offset))

#
# Slide the rectangle down to get the brightest reading
#
let offset=0
let ii=0
let maxreading=0
let reading=0

while ((ii<step))
do
	let yy=(rectY+ii)
	text=`convert ${tempImage} -crop ${rectW}x${rectH}+${rectX}+${yy} -resize 1x1 txt: | tail -1`
	result=`echo $text | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
	IFS=" "
	rgb=(`echo $result`) # R, G, B
	let reading=(${rgb[0]}+${rgb[1]}+${rgb[2]})
	((ii++))
	if ((reading>maxreading))
	then
		let maxreading=reading
		let offset=ii
	fi
done
((rectY+=offset))

convert ${tempImage} -fill none -stroke red -strokewidth 1 -draw "rectangle $rectX $rectY $((rectX+rectW)) $((rectY+rectH))" $tempImage

echo $rectX $rectY $((rectX+rectW)) $((rectY+rectH))
}

mktags()
{
#	findSyncLight $1
	if [[ ! -f tag.cfg ]]
	then
		(findSyncLight $1) | tee tag.cfg
	fi
    tagBox=($(head -1 tag.cfg)) # left, top, right, bottom 

	let x=${tagBox[0]}
	let y=${tagBox[1]}
    let width=$((${tagBox[2]} - ${tagBox[0]})) # (right - left)/2 + left
    let height=$((${tagBox[3]} - ${tagBox[1]})) # (right - left)/2 + left

	want="black"

	if [[ -f tagged.txt ]]
	then
		rm tagged.txt
	fi
#	set -o xtrace
	for ff in *.JPG
	do
		tag=`convert $ff -crop ${width}x${height}+${x}+${y} -resize 1x1 txt: | tail -1`
		echo $tag >> tagraw.txt
		result=`echo $tag | sed 's/[\(\),]/ /g;' | awk -F' ' '{print $3,$4,$5}'`
		IFS=" "
		coords=(`echo $result`)

		if [[ "green" == "$want" ]]
		then
			if [[ "${coords[0]}" -gt "100" && "${coords[1]}" -gt "100" && "${coords[2]}" -gt "100" ]]
			then
				echo $ff >> tagged.txt
				want="black"
			fi
		else
			if [[ "${coords[0]}" -lt "10" && "${coords[1]}" -lt "10" && "${coords[2]}" -lt "10" ]]
			then
				echo $ff >> tagged.txt
				want="green"
			fi
		fi
	done
}

gentagged()
{
	let offset=$1
	cat ../images/tagged.txt | while read ff
	do
		let gg=`echo $ff | cut -c5-10 | sed 's/^0*//g'`
 		ln -s ../images/SAM_`printf "%06d" $((gg+offset))`.JPG
	done
}

brighten()
{
	for ff in *.JPG; do convert $ff -modulate 140 $ff; done
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
	renumber2) renumber2 ;;
	rrenumber) rrenumber ;;
    preview) preview ;;
    #genyuv) genyuv web; genyuv dvd; genyuv hd ;;
    genyuv) genyuv dvd;;
    web) scaler web; gentitle web; mpeg2 web ;;
    #dvd) scaler dvd; gentitle dvd; mpeg2 dvd ;;
    dvd) scaler dvd; mpeg2 dvd ;;
    hd) mpeg2 hd ;;
    mklinks) mklinks ;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	mp42jpg) mp42jpg $2 ;;
	mktags) mktags $2;;
	gentagged) gentagged $2 ;;
	brighten) brighten ;;
	*) echo What? ;;
esac
