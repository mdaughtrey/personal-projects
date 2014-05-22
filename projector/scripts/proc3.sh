#!/bin/bash

let verbose=0
let simulate=0
let numTitleFrames=90
let fps=30
SCRIPT_DIR=~/scripts
SCRIPT_INTERPOLATE1=${SCRIPT_DIR}/pp_interpolate1.avs
SCRIPT_INTERPOLATE2=${SCRIPT_DIR}/pp_interpolate2.avs
SCRIPT_CLEAN1=${SCRIPT_DIR}/pp_clean1.avs
SCRIPT_CLEAN2=${SCRIPT_DIR}/pp_clean2.avs
let TITLE_STREAM_FRAMES=200
let NATIVE_WIDTH=5472
let NATIVE_HEIGHT=3648

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
	let to=${TITLE_STREAM_FRAMES}

	dir="cropped"
	numbers=$(ls ${dir}/SAM_*.JPG | xargs -I {} basename {} | cut -b5-10 | sort -n)
	for number in $numbers
	do
		filename=SAM_$(printf '%06u' $((10#$number))).JPG
		ln -s ${dir}/$filename SAM_$(printf '%06u' $to).JPG
		((to++))
	done
}

mkrlinks()
{
	let to=$(ls */*.JPG | wc -l)

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
			((to--))
		done
	done
}

renumber()
{
	let to=${1:-0}
	for from in $(ls SAM_????.JPG | cut -c5-8 | sort -n)
	do
		mv SAM_${from}.JPG SAM_`printf "%04d" ${to}`.JPG
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
    	doCommand ffmpeg -i SAM_%04d.JPG -b:v 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	else
    	doCommand ffmpeg -i SAM_%06d.JPG -b:v 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	fi
    mv $previewfile $HOME/imageinput/previews 
}

previewTitle()
{
    previewfile=${PWD//\//_}title_preview.mpg
	if [[ -f SAM_0000.JPG ]]
	then
    	doCommand ffmpeg -i SAM_00%02d.JPG -b:v 4000k -vf scale=1024:-1 -vcodec mpeg2video $previewfile
	fi
    doCommand mv $previewfile $HOME/imageinput/previews 
}

let scaleX=0
let scaleY=0
let width=0
let height=0 
let bw=4000
let xOffset=0
let yOffset=0

scaler()
{
	vOut === scaler
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

genyuv()
{
	vOut === genyuv
    scaler $1
    rm stream.yuv
    rm stream_${0}.yuv
	rm -f titlelist.txt
	rm -f contentlist.txt

	for ii in `seq 0 $((TITLE_STREAM_FRAMES-1))`
	do
		echo SAM_$(printf "%04u" $ii).JPG >> titlelist.txt
	done

	for ii in `seq ${TITLE_STREAM_FRAMES} $(($(ls SAM_*.JPG | wc -l)-1))`
	do
		echo SAM_$(printf "%04u" $ii).JPG >> contentlist.txt
	done

	options="-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub -vo yuv4mpeg" 
	if [[ "$2" == "inter3" ]]
	then
		cropoptions="-vf "
    	doCommand mplayer mf://@titlelist.txt ${options}:file=titlestream.yuv
	else
		cropoptions="-vf crop=$width:$height:${crop[0]}:${crop[1]},scale=$scaleX:$scaleY"
    	doCommand mplayer mf://@titlelist.txt ${cropoptions} ${options}:file=titlestream.yuv
	fi

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

	if [[ "-vf=" == "$cropoptions" ]]
	then
		cropoptions=""
	fi

	cropoptions=${cropoptions/ ,/}
	if [[ "-vf " == "$cropoptions" ]]
	then
		cropoptions=""
	fi

	vOut cropoptions $cropoptions
	options="${cropoptions} ${options}:file=contentstream.yuv"
    doCommand mplayer mf://@contentlist.txt ${options} 

	vOut Concatenating YUV Streams
	(cat titlestream.yuv; cat contentstream.yuv | (read junk; cat)) > stream_${1}.yuv
	vOut Concatenation Done
}

avi()
{
	vOut === avi
    if [[ ! -f stream_${1}.yuv ]]
    then
		vOut preGenyuv
        genyuv $1 $2
		vOut postGenyuv
    fi
    cat stream_${1}.yuv  | yuvfps -v 0 -r 18:1 -v 1 | ffmpeg -loglevel quiet -i - -vcodec rawvideo -y rawframes.avi
}

mpeg2()
{
    if [[ ! -f stream_${1}.yuv ]]
    then
        genyuv $1
    fi
    dvdfile=${PWD//\//_}dvd.mpg

    cat stream_${1}.yuv  | yuvfps -r ${fps}:1 -v 1 | mpeg2enc --multi-thread 4 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $dvdfile
    mv $dvdfile $HOME/imageinput/dvd
}

postprocess()
{
	vOut === postprocess
	majorMode=$1

	if [[ ! -f rawframes.avi ]]
	then
		avi dvd $majorMode
	fi

	case "$majorMode" in
		icompare) 
			RESULT=$(echo -n "result=\"resultS1\" # specify the wanted output here" )
			TEMPLATE=${SCRIPT_INTERPOLATE2}
			LOCALSCRIPT="interpolate_compare.avs"
			;;
		interpolate|inter3)
			RESULT=$(echo -n "result=\"result1\" # specify the wanted output here" )
			TEMPLATE=${SCRIPT_INTERPOLATE2}
			LOCALSCRIPT="interpolate.avs"
			;;
		clean)
			RESULT=$(echo -n "result=\"result1\" # specify the wanted output here" )
			TEMPLATE=${SCRIPT_CLEAN2}
			LOCALSCRIPT="clean.avs"
			;;
		ccompare)
			RESULT=$(echo -n "result=\"resultS1\" # specify the wanted output here" )
			TEMPLATE=${SCRIPT_CLEAN2}
			LOCALSCRIPT="clean_compare.avs"
			;;
    esac

	rawFrames=$(echo -n "film=\"Z:"; echo -n $PWD | sed 's/\//\\\\/g;'; echo "\\\\rawframes.avi\"")
	echo $rawFrames > ${LOCALSCRIPT}
	echo $RESULT >> ${LOCALSCRIPT}
    cat $TEMPLATE >> ${LOCALSCRIPT}
	wine avs2yuv.exe ${LOCALSCRIPT} - > out.yuv
	ffmpeg -loglevel verbose -i out.yuv  -b:v 4000K -y ${LOCALSCRIPT}.mpg 
    dvdfile=${PWD//\//_}_${majorMode}_dvd.mpg
    mv ${LOCALSCRIPT}.mpg $HOME/imageinput/dvd/${dvdfile}
}

gentitle()
{
	mkdir title
	FONT="/usr/share/fonts/truetype/droid/DroidSerif-BoldItalic.ttf"
	if [[ ! -f title.txt ]]
	then
		echo No title.txt
		exit 1
	fi
	type=${1:-"dvd"}
	scaler $type
	firstfile=${2:-"cropped/SAM_$(printf "%06u" $((10#$TITLE_STREAM_FRAMES))).JPG"}

	if [[ ! -f $firstfile ]]
	then
		echo no $firstfile found
		exit 1
	fi
	
	let rowsize=200
	let linecount=0
	let translateY=${crop[1]}
	
	compositecmd=$(cat title.txt | while read line
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
		echo -n " -page +${crop[0]}+$((translateY)) titleline_${linecount}.png"
		((linecount++))
	done) 

	compositecmd="convert ${compositecmd} -background transparent -layers merge titletext.png"
	echo $compositecmd
	$compositecmd
	rm titleline_*.png

	let translateX=${crop[0]}
	let translateY=${crop[1]}
	((translateX+=150))
	((translateY+=150))

	let NUMTASKS=$(cat /proc/cpuinfo | grep processor | wc -l)
#	((NUMTASKS*=2))

	for sepia in `seq 0 $((TITLE_STREAM_FRAMES-1))`
	do
#		(
		vOut Frame $sepia of ${TITLE_STREAM_FRAMES}
		inc=$(echo "scale=1;100/${TITLE_STREAM_FRAMES}" | bc -l)
		value=$(echo "${inc}*${sepia}" | bc -l)
		index=$(printf '%06u' $sepia)

		if [[ -f "SAM_${index}.JPG" ]]
		then
			continue
		fi

		option=""
		if [[ -f "mirror" ]]
		then
			option="-flop"
		fi

		doCommand convert $firstfile $option -blur 2x2 -modulate 100,${value} underlay_${index}.png
		doCommand convert -page +0+0 underlay_${index}.png -page +${translateX}+${translateY} \
			 titletext.png -layers merge SAM_${index}.JPG
		doCommand convert SAM_${index}.JPG -crop $((width+10))x$((height+10))+$((xOffset-5))+$((yOffset-5))  		tmp.jpg
		mv tmp.jpg  title/SAM_${index}.JPG
		doCommand rm underlay_${index}.png
#		) &
#		let numTasks=$(ps --no-headers --ppid $$ | wc -l)
#		if ((numTasks > NUMTASKS))
#		then
#			sleep 5
#		fi
	done
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
	rm -f *.avi *.avs *.yuv *.JPG *.png
	mklinks
	gentitle
	#renumber
	#previewtitle
	preview
	postprocess interpolate
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
		if [[ -f $template ]]
		then
			doCommand "rm $template"
		fi
	done
}

precrop()
{
	scaler
	mkdir cropped
	let to=${TITLE_STREAM_FRAMES}

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
			doCommand convert ${dir}PHOTO/${filename} -crop $((width+10))x$((height+10))+$((xOffset-5))+$((yOffset-5)) cropped/SAM_$(printf '%06u' $to).JPG
			((to++))
		done
	done
}

tonefuse()
{
	scaler
	outfile=enfuse.jpg
	let outindex=200
	dir="cropped"

	let baseindex=200

	while [[ -f "${dir}/SAM_$(printf "%06u" $baseindex).JPG" ]]
	do
		file1=SAM_$(printf "%06u" $baseindex)
		file2=SAM_$(printf "%06u" $((baseindex+1)))
		file3=SAM_$(printf "%06u" $((baseindex+2)))

		outfile=SAM_$(printf "%06u" $outindex).JPG
		vOut Tonefusing $outfile
		doCommand enfuse --output $outfile ${dir}/${file1}.JPG ${dir}/${file2}.JPG ${dir}/${file3}.JPG

		((outindex++))
		((baseindex+=3))
	done
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
	previewtitle) previewTitle ;;
    #genyuv) genyuv web; genyuv dvd; genyuv hd ;;
    genyuv) genyuv dvd;;
    web) scaler web; gentitle web; mpeg2 web ;;
    #dvd) scaler dvd; gentitle dvd; mpeg2 dvd ;;
    dvd) scaler dvd; mpeg2 dvd ;;
    hd) mpeg2 hd ;;
    mklinks) mklinks ;;
    mkrlinks) mkrlinks ;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	mp42jpg) mp42jpg $2 ;;
	mktags) mktags $2;;
	gentagged) gentagged $2 ;;
	avi) avi dvd ;;
	inter3|interpolate|[ic]compare|clean) postprocess $1 ;;
	drange) deleterange $2 $3 ;;
	all) all ;;
	precrop) precrop ;;
	tonefuse) tonefuse ;;
	*) echo What? ;;
esac
