#!/bin/bash

rm proc3_log.txt
set -o xtrace
date >> proc3_log.txt
exec 2>> proc3_log.txt

let verbose=0
let simulate=0
let clean=0
#let numTitleFrames=90
let fps=30

# logs command line invocations. Useful when you've forgotten what stage of the 
# process you were in
RUN_LOG=/home/mattd/documents/runlog.txt
SCRIPT_DIR=~/git/personal-projects/projector/scripts/
SCRIPT_INTERPOLATE1=${SCRIPT_DIR}/pp_interpolate1.avs
SCRIPT_INTERPOLATE2=${SCRIPT_DIR}/pp_interpolate2.avs
SCRIPT_CLEAN1=${SCRIPT_DIR}/pp_clean1.avs
SCRIPT_CLEAN2=${SCRIPT_DIR}/pp_clean2.avs
declare -ix TITLE_STREAM_FRAMES=200
#let TITLE_STREAM_FRAMES=2
let NATIVE_WIDTH=5472
let NATIVE_HEIGHT=3648
FONT="/usr/share/fonts/truetype/droid/DroidSerif-BoldItalic.ttf"
SEM="sem --will-cite"
DIRBASE_SYMLINKS=~/symlinks
DIRBASE_IMAGES=/mnt/imageinput
AVS2YUV=Z:\\mnt\\imageinput\\software\\avs2yuv\\avs2yuv.exe
# where to put the temporary video files 
YUVTMP=~/tmp/`basename $PWD`
FRAMETMP=/media/usb0/videotmp_`basename $PWD`
FFMPEG=avconv
LEVELS_TXT=levels.txt
LEVELS_ERROR=levelcheck.out
IMAGE_OPTIM="image_optim --no-pngout --no-advpng --no-optipng --no-pngquant  --no-svgo"

if [[ ! -d $YUVTMP ]]
then
	mkdir -p $YUVTMP
fi

symlinkSource=`pwd`
symlinkTarget=$DIRBASE_SYMLINKS/${PWD//$DIRBASE_IMAGES/}

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

#mklinks()
#{
#	let to=0
#	dir="title"
#	if [[ "$optionM" == "1" && ! -d markers ]]
#	then
#		mkdir markers
#	fi
#
#	if [[ ! -d $symlinkTarget ]]
#	then
#		mkdir -p $symlinkTarget
#	fi
#
#	let to=${TITLE_STREAM_FRAMES}
#	dir="fused"
#
#	numbers=$(ls ${dir}/SAM_*.JPG | xargs -I {} basename {} | cut -b5-10 | sort -n)
#	for number in $numbers
#	do
#		if (($to % 500 == 0))
#		then
#			if [[ "$optionM" == "1" ]]
#			then
#				markerfile=markers/marker_$(printf '%06u' $((10#$number))).JPG
#				if [[ ! -f $markerfile ]]
#				then
#					textToImage $markerfile ${dir}/$filename
#				fi
#				ln -s $markerfile $symlinkTarget/SAM_$(printf '%06u' $to).JPG
#				((to++))
#			fi
#		fi
#		filename=SAM_$(printf '%06u' $((10#$number))).JPG
#		ln -s ${dir}/$filename $symlinkTarget/SAM_$(printf '%06u' $to).JPG
#		((to++))
#	done
#}
#
#mkrlinks()
#{
#	let to=$(ls */*.JPG | wc -l)
#
#	for ff in *PHOTO/SAM_0*.JPG
#    do
#    	mv $ff $(echo $ff | sed 's/_0/_1/')
#	done
#	dirs=$(ls -d *PHOTO | sed 's/PHOTO//' | sort -n)
#
#	for dir in $dirs
#	do
#		numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {} | cut -b5-8 | sort -n)
#
#		for number in $numbers
#		do
#			filename=SAM_$(printf '%04u' $number).JPG
#			ln -s ${dir}PHOTO/$filename SAM_$(printf '%04u' $to).JPG
#			((to--))
#		done
#	done
#}

#renumber()
#{
#	let to=${1:-0}
#	for from in $(ls SAM_??????.JPG | cut -c5-10 | sort -n)
#	do
#		mv SAM_${from}.JPG SAM_`printf "%06u" ${to}`.JPG
#		((to++))
#	done
#}


preview()
{
	rm -f *.JPG *.png 
	rm -rf title cropped fused
    rm -rf $YUVTMP
	mkdir -p $YUVTMP
	let TITLE_STREAM_FRAMES=36
	precrop 720
#	optimize
	tonefuse
	gentitle 
	postprocess icompare
    rm -rf $YUVTMP
}

previewTitle()
{
    previewfile=${PWD//\//_}title_preview.mpg
	if [[ -f SAM_0000.JPG ]]
	then
    	doCommand $FFMPEG -i SAM_00%02d.JPG -b 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	fi
    doCommand mv $previewfile $HOME/imageinput/previews 
}

let scaleX=0
let scaleY=0
#let width=0
declare -ix width=0
#let height=0 
declare -ix height=0 
let bw=4000
#let xOffset=0
declare -ix xOffset=0
#let yOffset=0
declare -ix yOffset=0
let origYOffset=0
let origXOffset=0

scaler()
{
	vOut === scaler
	cropfile=${1:-crop.cfg}
#    if [[ "$1" == "web" ]]
#    then
#        let scaleX=300
#    fi
#    if [[ "$1" == "dvd" ]]
#    then
#        let scaleX=720
#    fi
#    if [ "$1" == 'hd' ]
#    then
#        let bw=8000
#        let scaleX=2048
#    fi
    crop=($(head -1 $cropfile)) # left, top, right, bottom 

	let xOrigOffset=${crop[0]}
	let yOrigOffset=${crop[1]}
	let xOffset=${crop[0]}
	let yOffset=${crop[1]}
    let width=$((${crop[2]} - ${crop[0]})) # (right - left)/2 + left
    let height=$((${crop[3]} - ${crop[1]})) # (right - left)/2 + left

#	if [[ -f "flip" ]]
#	then
#		vOut flip option set
#		options="${options},flip"
#	fi
    vOut XXXBefore Scaler L: ${crop[0]} T: ${crop[1]} R: ${crop[2]} B: ${crop[3]}
	if [[ -f "mirror" ]]
	then
		let distanceLeft=${crop[0]}
		let distanceRight=$(($NATIVE_WIDTH-${crop[2]}))
		let crop[0]=$distanceRight
		let crop[2]=$((${crop[0]}+${width}))
		vOut mirror option set
	fi
#    vOut XXXAfter Scaler L: ${crop[0]} T: ${crop[1]} R: ${crop[2]} B: ${crop[3]}
#	exit

    scaler=$(echo "scale=2;${width}/${height}" | bc) 
    let scaleY=$(echo "$scaleX/$scaler" | bc)

    vOut Scaler width $width height $height scaleX $scaleX scaleY $scaleY bw $bw
}

# Converts the images in the title/ subdirectory into titlestream.yuv
# Converts the images in the tonefuse/ subdir into contentstream.yuv
# Concatenates those into stream_dvd.uv
# Images are scaled to DVD resolution here, also flipping and mirroring as necessary
genyuv()
{
	vOut === genyuv
	scaler # $1
	rm $YUVTMP/title_stream.yuv
	rm $YUVTMP/stream_${1}.yuv
	rm -f titlelist.txt
	rm -f contentlist.txt

	for ii in `seq 0 $((TITLE_STREAM_FRAMES-1))`
	do
		echo title/SAM_$(printf "%06u" $ii).JPG >> titlelist.txt
	done

	let end=$(($(ls fused/SAM_*.JPG | wc -l)-1))
	((end+=$TITLE_STREAM_FRAMES))
	#for ii in `seq ${TITLE_STREAM_FRAMES} $(($(ls fused/SAM_*.JPG | wc -l)-1))`
	for ii in `seq ${TITLE_STREAM_FRAMES} $end`
	do
		echo fused/SAM_$(printf "%06u" $ii).JPG >> contentlist.txt
	done

	options="-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub -vo yuv4mpeg" 
#	if [[ "$2" == "inter3" ]]
#	then
		cropoptions="-vf scale=$scaleX:$scaleY"
	#	cropoptions="-vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY"
    	doCommand mplayer -lavdopts threads=`nproc` mf://@titlelist.txt $cropoptions ${options}:file=$YUVTMP/titlestream.yuv
#	else
##		cropoptions="-vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY"
##    	doCommand mplayer mf://@titlelist.txt ${cropoptions} ${options}:file=titlestream.yuv
##	fi

	if [[ -f "flip" ]]
	then
		vOut flip option set
		cropoptions="${cropoptions},flip"
	fi

	if [[ -f "mirror" ]]
	then
		vOut mirror option set
		cropoptions="${cropoptions},mirror"
	fi

#	cropoptions=${cropoptions/ ,/ }
#	if [[ "-vf " == "$cropoptions" ]]
#	then
#		cropoptions=""
#	fi

	vOut cropoptions $cropoptions
	options="${cropoptions} ${options}:file=$YUVTMP/contentstream.yuv"
    doCommand mplayer -lavdopts threads=`nproc` mf://@contentlist.txt ${options} 

	vOut Concatenating YUV Streams
	(cat $YUVTMP/titlestream.yuv; cat $YUVTMP/contentstream.yuv | (read junk; cat)) > $YUVTMP/stream_${1}.yuv
	vOut Concatenation Done
}

# Converts stream_dvd.yuv into rawframes.avi. Framerate conversion from 18 to 30fps happens here
avi()
{
    vOut === avi
    if [[ ! -f $YUVTMP/stream_${1}.yuv ]]
    then
        genyuv $1 $2
    fi
    cat $YUVTMP/stream_${1}.yuv  | yuvfps -v 0 -r 18:1 -v 1 | $FFMPEG -loglevel quiet -i - -vcodec rawvideo -y $YUVTMP/rawframes.avi
}

# YUV files converted to mpeg, copied to final destination
mpeg2()
{
    if [[ ! -f $YUVTMP/stream_${1}.yuv ]]
    then
        genyuv $1
    fi
    dvdfile=${PWD//\//_}dvd.mpg

    cat $YUVTMP/stream_${1}.yuv  | yuvfps -r ${fps}:1 -v 1 | mpeg2enc --multi-thread 4 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $dvdfile
    mv $dvdfile $HOME/imageinput/dvd
}

postprocess()
{
	vOut === postprocess
	majorMode=$1
	if ((clean == 1))
	then
		rm $YUVTMP/*.yuv $YUVTMP/*.av? *.av? *.mpg
	fi

	if [[ ! -f $YUVTMP/rawframes.avi ]]
	then
		avi dvd $majorMode
	fi

	case "$majorMode" in
	icompare) 
	RESULT=$(echo -n "result=\"resultS1\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_INTERPOLATE2}
	LOCALSCRIPT="$YUVTMP/interpolate_compare.avs"
	;;
	interpolate|inter3)
	RESULT=$(echo -n "result=\"result1\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_INTERPOLATE2}
	LOCALSCRIPT="$YUVTMP/interpolate.avs"
	;;
	clean)
	RESULT=$(echo -n "result=\"resultclean\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_CLEAN2}
	LOCALSCRIPT="$YUVTMP/clean.avs"
	;;
	ccompare)
	RESULT=$(echo -n "result=\"resultS1\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_CLEAN2}
	LOCALSCRIPT="$YUVTMP/clean_compare.avs"
	;;
	esac

	if [[ ! -f "$YUVTMP/out.yuv" ]]
	then
			rawFrames=$(echo -n "film=\"Z:\\\\home\\\\mattd\\\\tmp\\\\`basename $PWD`\\\\rawframes.avi\"")
			echo $rawFrames > ${LOCALSCRIPT}
			echo $RESULT >> ${LOCALSCRIPT}
			cat $TEMPLATE >> ${LOCALSCRIPT}
			wine $AVS2YUV ${LOCALSCRIPT} - > $YUVTMP/out.yuv
	fi
	#$FFMPEG -loglevel verbose -i $YUVTMP/out.yuv -threads `nproc` -b 4000K -y ${LOCALSCRIPT}.mpg 
	$FFMPEG -loglevel verbose -i $YUVTMP/out.yuv -threads `nproc`  -b:v 4M -maxrate 4M -minrate 4M -bufsize 4M  -y ${LOCALSCRIPT}.mpg 
    dvdfile=${PWD//\//_}_${majorMode}_dvd.mpg
    mv ${LOCALSCRIPT}.mpg /mnt/imageinput/dvd/${dvdfile}
}

oneTitleFrame()
{
	sepia=$1
	translateX=$2
	translateY=$3
	titleFile=$4
	firstfile="fused/SAM_$(printf '%06u' $TITLE_STREAM_FRAMES).JPG"
	inc=$(echo "scale=1;100/${TITLE_STREAM_FRAMES}" | bc -l)
	value=$(echo "${inc}*${sepia}" | bc -l)
	index=$(printf '%06u' $sepia)
	if [[ -f "flip" ]]
	then
		flipOption='-flip'
	fi

	eval "convert $firstfile $flipOption -blur 2x2 -modulate 100,${value} underlay_${index}.png"
	convert underlay_${index}.png -page +${translateX}+${translateY} \
		 $titleFile -layers merge title/SAM_${index}.JPG
#	ln -s title/SAM_${index}.JPG
}
export -f oneTitleFrame

textToImage()
{
	convert -background black \
		-stroke yellow -strokewidth 2 \
		-fill blue -font ${FONT} \
		-size x100 label:"${2}" /tmp/$(basename $1)
	geometry=($(identify -ping $firstfile  | cut -d' ' -f3 | sed 's/x/ /g'))
	let underlayW=${geometry[0]}
	let underlayH=${geometry[1]} 
}

gentitle()
{
	mkdir title
	if [[ ! -f title.txt ]]
	then
		echo No title.txt
		exit 1
	fi
	type=${1:-"dvd"}
	scaler #$type
	firstfile=${2:-"fused/SAM_$(printf '%06u' $TITLE_STREAM_FRAMES).JPG"}

	if [[ ! -f $firstfile ]]
	then
		echo no $firstfile found
		exit 1
	fi
	
	let rowsize=200
	let linecount=0
	let translateX=0
	let translateY=0
	let pageIndex=0

	for pageFile in title.txt title[123456789].txt
	do
		if [[ ! -f $pageFile ]]
		then
			continue
		fi

		echo Generate Title Page $pageIndex
	
		compositecmd=$(cat $pageFile | while read line
		do
			font=${font// /-}
			if [[ "=" == "$line" ]]
			then
				((rowsize-=rowsize/3))
				continue
			fi

			if [[ "" == "$line" ]]
			then
				((translateY+=rowsize+20))
				continue
			fi

			convert -background transparent \
				-stroke yellow -strokewidth 2 \
				-fill blue -font ${FONT} \
				-size x${rowsize} label:"${line}" titleline_${linecount}.png
			((translateY+=rowsize+20))
			echo -n " -page +$((translateX))+$((translateY)) titleline_${linecount}.png"
			((linecount++))
		done) 

		compositecmd="convert ${compositecmd} -background transparent -layers merge titletext${pageIndex}.png"
		echo $compositecmd
		$compositecmd
		rm titleline_*.png
		((pageIndex++))
    done	

	# get the underlay geometry
	geometry=($(identify -ping $firstfile  | cut -d' ' -f3 | sed 's/x/ /g'))
	let underlayW=${geometry[0]}
	let underlayH=${geometry[1]} 

	let translateX=0
	let translateY=0

	# number of frames each title page gets
	let pagePartition=$(echo "scale=0;$TITLE_STREAM_FRAMES/$pageIndex" | bc) 
	let renderStart=0
	let renderEnd=$(($pagePartition-1))
	let pageIndex=0
	while ((renderStart < $(($TITLE_STREAM_FRAMES-1))))
	do
		geometry=($(identify -ping titletext${pageIndex}.png  | cut -d' ' -f3 | sed 's/x/ /g'))
		let titleW=${geometry[0]}
		let titleH=${geometry[1]} 
		let translateX=$(($underlayW/2 - $titleW/2))
		let translateY=$(($underlayH/2 - $titleH/2))
		for renderIndex in `seq $renderStart $renderEnd`
		do
			$SEM -N0 --jobs 200%  oneTitleFrame $renderIndex $translateX $translateY titletext${pageIndex}.png
		done
		((renderStart=$renderEnd))
		((renderEnd+=$pagePartition))
		if (($renderEnd > $(($TITLE_STREAM_FRAMES-1))))
		then
			let renderEnd=$(($TITLE_STREAM_FRAMES-1))
		else
			((pageIndex++))
		fi
	done
#	sem --wait
}

mp42jpg()
{
	if [[ "" == "$1" ]]
	then
		echo No file
		return
	fi
	$FFMPEG -i $1 -b 2000 -qscale 1 -qcomp 0 -qblur 0 SAM_%06d.JPG
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

all()
{
#    rm -rf $YUVTMP
	rm -f *.JPG *.png 
	rm -rf title cropped fused
	echo precrop
	precrop
	echo optimize
	optimize
	echo tonefuse
	tonefuse
	echo gentitle
	gentitle
	echo interpolate
	postprocess interpolate
	rm $YUVTMP/out.yuv
	echo clean
	postprocess clean
}

deleterange()
{
	if [[ "" == "$2" ]]
	then
		vOut enter range
		return
	fi
	for ff in `seq $1 $2`
	do
		template="SAM_$(printf '%06u' $ff).JPG"
		if [[ -f "$template" ]]
		then
			doCommand "rm $template"
		fi
	done
}

onePrecrop()
{
# index dir filename width height xOffset yOffset 
#.	levelfile=$(tempfile -d /tmp -p lvlck)
	let index=$(echo -n 10#$1)
	outfile="cropped/SAM_$(printf '%06u' $index).JPG"
	#(echo -n $1" "; convert ${2}PHOTO/${3} -crop ${4}x${5}+${6}+${7} - | tee $outfile | identify -format '%[mean]' -)  > $levelfile
	convert ${2}PHOTO/${3} -crop ${4}x${5}+${6}+${7} $outfile
}

export -f onePrecrop

precrop()
{
	touch mirror
	let numFrames=${1:-999999}
	scaler
#	rm /tmp/lvlck*
	if ((clean == 1))
	then
		rm -rf cropped
	fi
	if [[ ! -d cropped ]]
	then
		mkdir cropped
	fi
	let to=${TITLE_STREAM_FRAMES}
	((numFrames+=2))

	dirs=$(ls -d *PHOTO | sed 's/PHOTO//' | sort -n)

	for dir in $dirs
	do
		numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {} | cut -b5-8 | sort -n)
		if [[ -f $dir/crop.cfg ]]
		then
			scaler $dir/crop.cfg
#		else
#			scaler ../crop.cfg
		fi

		for number in $numbers
		do
			echo Frame $number
			let index=$(echo -n 10#$to)
			outfile="cropped/SAM_$(printf '%06u' $to).JPG"
			#levelfile=$(tempfile -d /tmp -p lvlck)
			#outfile="/tmp/SAM_$(printf '%06u' $to).JPG"
			if [[ ! -f $outfile ]]
			then
				filename=SAM_$(printf '%04u' $((10#$number))).JPG
				$SEM -N0 --jobs 200% onePrecrop $index $dir $filename $width $height $xOffset $yOffset 
				#onePrecrop $index $dir $filename $width $height $xOffset $yOffset 
			fi
			((to++))
			if ((to == numFrames))
			then
				$SEM --wait
				return
			fi
		done
	done
	$SEM --wait
#	cat /tmp/lvlck* | sort -nk1 > levels.txt
#	rm /tmp/lvlck*
}

# optional for mac only it seems
optimize ()
{
	for ff in cropped/*.JPG
	do
		$SEM -N0 --jobs 200% $IMAGE_OPTIM $ff
	done
}

#
# Functions resync, levelsMismatch, toneCheck
# - these routines check the levels.txt file generated by precrop to make sure that the cropped
# files are properly organized as exposure triples (ie, darkest, middle, brightest).
#
declare -A explevels

function resync()
{
	let base=$1
	while ((${explevels[$base]} >= ${explevels[$((base+1))]}    
		|| ${explevels[$((base+1))]} >= ${explevels[$((base+2))]}
		|| 0 == ${explevels[$base]}
		|| 0 == ${explevels[$((base+1))]}
		|| 0 == ${explevels[$((base+2))]}))
	do
		((base++))
	done
	echo "Resynced at $base" >> $LEVELS_ERROR
	echo $base
}

function explevelsMismatch()
{
	let base=$1
	echo -n "Levels Mismatch at $base $((base+1)) $((base+2)) " >> $LEVELS_ERROR
	echo "${explevels[$base]} ${explevels[$((base+1))]} ${explevels[$((base+2))]}" >> $LEVELS_ERROR
	echo $(resync $base)
}

toneCheck()
{
	if [[ ! -f "$LEVELS_TXT" ]]
	then
		echo No $LEVELS_TXT file
		exit 1
	fi

	rm $LEVELS_ERROR 2>/dev/null

	for line in `seq 1 $(cat $LEVELS_TXT | wc -l)`
	do
		read index leveltxt <<< $(cat $LEVELS_TXT | sed -ne "${line}p;")
		((index-=200))
		let level=${leveltxt%%.*}
#		if ((0 == $level)); then echo -n '!'; fi
		((explevels[$index] = $level))
    done

	let end=${#explevels[@]}
	let ii=0
	while (($ii < $end))
	do
		if (($ii + 3 >= $end))
		then
			echo "Not enough triples, terminating at $ii" >> $LEVELS_ERROR
			exit 1
		fi
		let next=$((ii + 1))
		let after=$((ii + 2))
		if ((${explevels[$ii]} < ${explevels[$next]} && ${explevels[$next]} < ${explevels[$after]}))
		then
			((ii += 3))
			continue
		fi
		let ii=$(explevelsMismatch $ii)
	done
}


oneToneFuse()
{
	dir=$1
	baseindex=$2
	outindex=$3
	file1=SAM_$(printf "%06u" $baseindex)
	file2=SAM_$(printf "%06u" $((baseindex+1)))
	file3=SAM_$(printf "%06u" $((baseindex+2)))

	outfile=fused/SAM_$(printf "%06u" $outindex).JPG
	enfuse --output $outfile ${dir}/${file1}.JPG ${dir}/${file2}.JPG ${dir}/${file3}.JPG
}
export -f oneToneFuse

tonefuse()
{
	scaler
	let outindex=$TITLE_STREAM_FRAMES

	if ((clean == 1))
	then 
		rm -rf fused
	fi
	if [[ ! -d fused ]]
	then
		mkdir fused
	fi
	let baseindex=$TITLE_STREAM_FRAMES

	while [[ -f "cropped/SAM_$(printf "%06u" $baseindex).JPG" ]]
	do
		vOut Tonefusing $outindex
		outfile="$fused/SAM_$(printf "%06u" $((baseindex+3)))"
		if [[ ! -f $outfile ]]
		then
			#sem -N0 --jobs 200% oneToneFuse $dir $baseindex $outindex
			$SEM -N0 --jobs 200% oneToneFuse cropped $baseindex $outindex
		fi

		((outindex++))
		((baseindex+=3))
	done
	$SEM --wait
}

oneCropFuse()
{
	dir=$1
	file1=$2
	file2=$3
	file3=$4
	outfile=$5
	convert ${dir}PHOTO/${file1} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file1
	convert ${dir}PHOTO/${file2} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file2
	convert ${dir}PHOTO/${file3} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file3
	enfuse --output $outfile /tmp/$file1 /tmp/$file2 /tmp/$file3 
	rm /tmp/$file1 /tmp/$file2 /tmp/$file3
}
export -f oneCropFuse

cropfuse()
{
	scaler
	if ((clean == 1))
	then 
		rm -rf fused
	fi
	if [[ ! -d fused ]]
	then
		mkdir fused
	fi
	dirs=$(ls -d *PHOTO | sed 's/PHOTO//' | sort -n)
	let baseindex=$TITLE_STREAM_FRAMES
	
	for dir in $dirs
	do
		#numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {} | cut -b5-8 | sort -n)
		numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {})
		let outindex=$baseindex
		((baseindex+=$(echo $numbers | wc -w)/3))
		echo $numbers | sed 's/[^ ]\+ [^ ]\+ [^ ]\+ /&\n/g' | while read file1 file2 file3
		do
			outfile=fused/SAM_$(printf "%06u" $outindex).JPG
			((outindex++))
			if [[ -f $outfile ]]
			then
				continue
			fi
			if [[ "$file1" == "" || "$file2" == "" || "$file3" == "" ]]
			then
				return 0
			fi
			$SEM -N0 --jobs 200% oneCropFuse $dir $file1 $file2 $file3 $outfile
		done
	done
	$SEM --wait
}

import()
{
    mkdir -p $FRAMETMP
	rsync -rav ???PHOTO $FRAMETMP/
	cp crop.cfg $FRAMETMP
	cp title*.txt $FRAMETMP
}

(date | tr -d '\n'; echo -n " "; pwd | tr -d '\n'; echo -n " ";  echo $*) >> $RUN_LOG

while getopts "msvC" OPT
do
    case $OPT in
        v) let verbose=1 ;;
        s) let simulate=1 ;;
		C) let clean=1 ;;
		m) let optionM=1 ;;
        *) echo What?; exit 1 ;;
    esac
done
shift $((OPTIND-1))
#echo $@
case "$1" in 
	title) gentitle $2 ;;
	#renumber) renumber $2 ;;
    preview) preview ;;
	previewtitle) previewTitle ;;
    #genyuv) genyuv web; genyuv dvd; genyuv hd ;;
    genyuv) genyuv dvd;;
    #web) scaler web; gentitle web; mpeg2 web ;;
    #dvd) scaler dvd; gentitle dvd; mpeg2 dvd ;;
    #dvd) scaler dvd; mpeg2 dvd ;;
    #hd) mpeg2 hd ;;
    #mklinks) mklinks ;;
    #mkrlinks) mkrlinks ;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	mp42jpg) mp42jpg $2 ;;
	mktags) mktags $2;;
	gentagged) gentagged $2 ;;
	avi) avi dvd ;;
	inter3|interpolate|[ic]compare|clean) postprocess $1 ;;
	drange) deleterange $2 $3 ;;
	all) all ;;
	precrop) precrop ;;
	optimize) optimize ;;
	tonecheck) toneCheck ;;
	tonefuse) tonefuse ;;
	cropfuse) cropfuse ;;
	import) import ;;
	*) echo What? ;;
esac

#
# precrop
# tonefuse


#!/bin/bash
#filename=SAM_007281.JPG
#let index=$(echo -n '10#'; echo $filename | cut -c5-10)
#
#(echo -n $index" "; convert SAM_007281.JPG -crop 200x200+200+100 - | tee out.jpg  |  identify -format '%[mean]' - ) > levels.txt
				#$SEM -N0 --jobs 200% convert ${dir}PHOTO/${filename} \
				# -crop ${width}x${height}+${xOffset}+${yOffset} $outfile
				#$SEM -N0 --jobs 200% (echo -n $index" "; convert ${dir}PHOTO/${filename} -crop ${width}x${height}+${xOffset}+${yOffset} | tee $outfile | identify -format '%[mean]' -  > $levelfile)
# index dir filename width height xOffset yOffset 
#				$SEM -N0 --jobs 200% echo -n $index" "; convert ${dir}PHOTO/${filename} -crop ${width}x${height}+${xOffset}+${yOffset} | tee $outfile | identify -format '%[mean]' -  > $levelfile
#				$SEM -N0 --jobs 200% onePrecrop $index $filename $dir $width $height $xOffset $yOffset $outfile

