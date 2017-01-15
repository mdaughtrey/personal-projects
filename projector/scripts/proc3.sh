#!/bin/bash

#rm proc3_log.txt
set -o xtrace
date >> proc3_log.txt
exec 2>> proc3_log.txt

let verbose=1
let simulate=0
let clean=0
#let numTitleFrames=90
let fps=30
let frameLimit=0

# logs command line invocations. Useful when you've forgotten what stage of the 
# process you were in
RUN_LOG=/home/mattd/documents/runlog.txt
HD1=${P3HD1:/mnt/temphd}
HD2=${P3HD2:/mnt/temphd2}
SCRIPT_DIR=${P3SCRIPTDIR:~/git/personal-projects/projector/scripts/}
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
#DIRBASE_SYMLINKS=~/symlinks
DIRBASE_IMAGES=/mnt/imageinput
AVS2YUV=Z:\\mnt\\imageinput\\software\\avs2yuv\\avs2yuv.exe
# where to put the temporary video files 
#YUVTMP=~/tmp/`basename $PWD`
YUVTMP=$PWD
PROCDIR1=`pwd`
PROCDIR2=${PROCDIR1/$HD1/$HD2}
FFMPEG=avconv
LEVELS_TXT=levels.txt
LEVELS_ERROR=levelcheck.out
IMAGE_OPTIM="image_optim --no-pngout --no-advpng --no-optipng --no-pngquant  --no-svgo"
#IMAGE_OPTIM="jpegoptim -o"

touch mirror
#touch flip

if [[ ! -d $YUVTMP ]]
then
	mkdir -p $YUVTMP
fi

#symlinkSource=`pwd`
#symlinkTarget=$DIRBASE_SYMLINKS/${PWD//$DIRBASE_IMAGES/}

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

renumber()
{
	let to=${1:-0}
	for from in $(ls SAM_??????.JPG | cut -c5-10 | sort -n)
	do
		echo mv -u SAM_${from}.JPG SAM_`printf "%06u" ${to}`.JPG
		((to++))
	done
}


#preview()
#{
#	rm -f *.JPG *.png 
#	rm -rf title cropped fused
#    rm -rf $YUVTMP
#	mkdir -p $YUVTMP
#	let TITLE_STREAM_FRAMES=36
#	import
#	precrop 720
##	optimize
#	#tonefuse
#	gentitle 
#	postprocess icompare
#    rm -rf $YUVTMP
#}

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
	#scaler # $1
	rm ./title_stream.yuv
	rm ./stream_${1}.yuv
	rm -f titlelist.txt
	rm -f contentlist.txt

	for ii in `seq 0 $((TITLE_STREAM_FRAMES-1))`
	do
		echo title/SAM_$(printf "%06u" $ii).JPG >> titlelist.txt
	done

	let end=$(($(ls ${PROCDIR}/fused/SAM_*.JPG | wc -l)-1))
    if ((frameLimit != 0))
    then
        let end=$frameLimit
    fi
	((end+=$TITLE_STREAM_FRAMES))
    sourceDir=${PROCDIR2}/fused
    #if [[ "$2" == 'bw' ]]
    #then
    #    sourceDir=bw
    #fi
	for ii in `seq ${TITLE_STREAM_FRAMES} $end`
	do
		#echo fused/SAM_$(printf "%06u" $ii).JPG >> contentlist.txt
		echo ${sourceDir}/SAM_$(printf "%06u" $ii).JPG >> contentlist.txt
	done

	options="-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub -vo yuv4mpeg" 
	if [[ -f "flip" ]]
	then
		vOut flip option set
		cropoptions="${cropoptions},flip"
	fi

    cropoptions="-vf scale=800:600"

   	doCommand mplayer -lavdopts threads=`nproc` mf://@titlelist.txt $cropoptions ${options}:file=./titlestream.yuv

	vOut cropoptions $cropoptions
	options="${cropoptions} ${options}:file=./contentstream.yuv"
    doCommand mplayer -msglevel all=6 -lavdopts threads=`nproc` mf://@contentlist.txt ${options} 

	vOut Concatenating YUV Streams
	(cat ./titlestream.yuv; cat ./contentstream.yuv | (read junk; cat)) > ${PROCDIR2}/stream_${1}.yuv
	vOut Concatenation Done
}

# Converts stream_dvd.yuv into rawframes.avi. Framerate conversion from 18 to 30fps happens here
avi()
{
    vOut === avi
    if [[ ! -f ${PROCDIR2}/stream_${1}.yuv ]]
    then
        genyuv $1 $2
    fi
    cat ${PROCDIR2}/stream_${1}.yuv  | yuvfps -v 0 -r 18:1 -v 1 | $FFMPEG -loglevel info -i - -vcodec rawvideo -y ./rawframes.avi
}

# YUV files converted to mpeg, copied to final destination
mpeg2()
{
    if [[ ! -f ${PROCDIR2}/stream_${1}.yuv ]]
    then
        genyuv $1
    fi
    dvdfile=${PWD//\//_}dvd.mpg

    cat ${PROCDIR2}/stream_${1}.yuv  | yuvfps -r ${fps}:1 -v 1 | mpeg2enc --multi-thread 4 -f 0 -a 1 -b $bw -V 3000 -q 1 -o $dvdfile
    mv $dvdfile $HOME/imageinput/dvd
}

postprocess()
{
	vOut === postprocess
	majorMode=$1
	if ((clean == 1))
	then
		rm ./out.yuv
	fi

	if [[ ! -f ./rawframes.avi ]]
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
	clean|bw)
	RESULT=$(echo -n "result=\"resultclean\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_CLEAN2}
	LOCALSCRIPT="$YUVTMP/clean.avs"
	;;
	ccompare)
	RESULT=$(echo -n "result=\"resultS1\" # specify the wanted output here" )
	TEMPLATE=${SCRIPT_CLEAN2}
	LOCALSCRIPT="$YUVTMP/clean_compare.avs"
	;;
    cresultS?)
	RESULT=$(echo -n "result=\"${majorMode:1}\"")
	TEMPLATE=${SCRIPT_CLEAN2}
	LOCALSCRIPT="$YUVTMP/pptest_${majorMode}.avs"
	;;
    iresultS?)
	RESULT=$(echo -n "result=\"${majorMode:1}\"")
	TEMPLATE=${SCRIPT_INTERPOLATE2}
	LOCALSCRIPT="$YUVTMP/pptest_${majorMode}.avs"
	;;
	esac

	if [[ ! -f "$YUVTMP/out.yuv" ]]
	then
			rawFrames=$(echo -n "film=\"Y:\\\\`basename $PWD`\\\\rawframes.avi\"")
			echo $rawFrames > "${LOCALSCRIPT}"
			echo $RESULT >> "${LOCALSCRIPT}"
			cat $TEMPLATE >> "${LOCALSCRIPT}"
			wine $AVS2YUV "${LOCALSCRIPT}" - > $YUVTMP/out.yuv
	fi

avconv -loglevel verbose -y -i $YUVTMP/out.yuv -threads `nproc` -f mp4 -vcodec libx264 -preset slow -b:v 4000k  -flags +loop -cmp chroma -b:v 1250k -maxrate 4500k -bufsize 4M -bt 256k -refs 1 -bf 3 -coder 1 -me_method umh -me_range 16 -subq 7 -partitions +parti4x4+parti8x8+partp8x8+partb8x8 -g 250 -keyint_min 25 -level 30 -qmin 10 -qmax 51 -qcomp 0.6 -trellis 2 -sc_threshold 40 -i_qfactor 0.71 -acodec aac -strict experimental -b:a 112k -ar 48000 -ac 2 ${LOCALSCRIPT}.mp4

    dvdfile=${PWD//\//_}_${majorMode}_dvd.mp4
    mv ${LOCALSCRIPT}.mp4 /mnt/imageinput/dvd/${dvdfile}
}

oneTitleFrame()
{
	sepia=$1
	translateX=$2
	translateY=$3
	titleFile=$4
	#firstfile="fused/SAM_$(printf '%06u' $TITLE_STREAM_FRAMES).JPG"
	firstfile="fused/SAM_$(printf '%06u' $TITLE_STREAM_FRAMES).JPG"
	inc=$(echo "scale=1;100/${TITLE_STREAM_FRAMES}" | bc -l)
	value=$(echo "${inc}*${sepia}" | bc -l)
	index=$(printf '%06u' $sepia)
#	if [[ -f "flip" ]]
#	then
#		flipOption='-flip'
#	fi

	eval "convert $firstfile $flipOption -blur 2x2 -modulate 100,${value} underlay_${index}.png"
	convert underlay_${index}.png -page +${translateX}+${translateY} \
		 $titleFile -layers merge title/SAM_${index}.JPG
#	ln -s title/SAM_${index}.JPG
}
export -f oneTitleFrame

#textToImage()
#{
#	convert -background black \
#		-stroke yellow -strokewidth 2 \
#		-fill blue -font ${FONT} \
#		-size x100 label:"${2}" /tmp/$(basename $1)
#	geometry=($(identify -ping $firstfile  | cut -d' ' -f3 | sed 's/x/ /g'))
#	let underlayW=${geometry[0]}
#	let underlayH=${geometry[1]} 
#}

gentitle()
{
	mkdir title
	if [[ ! -f title.txt ]]
	then
		echo No title.txt
		exit 1
	fi
	type=${1:-"dvd"}

	firstfile=${2:-"${PROCDIR2}/fused/SAM_$(printf '%06u' $TITLE_STREAM_FRAMES).JPG"}

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
            vOut Title $renderIndex
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

numbercheck()
{
    let good=1
    for dir in `ls -d ???PHOTO`
    do
        numfiles=$(ls $dir/*.JPG | wc -l)
        if ((($numfiles / 3) * 3 != $numfiles))
        then
            echo "$dir frame count is not a multiple of 3: $numfiles no good"
            let good=0
        else
            echo $dir has $numfiles frames
        fi 
    done
    return $good
}

split()
{
    let numdirs=$(ls *.JPG | wc -l)
    let numdirs=$((numdirs/1000))

    for ((ii = 0; ii <= $numdirs; ii++))
    do
        thisdir=$(printf '%03u' $ii)
        mkdir $thisdir
        mv $(printf 'SAM_%03u*.JPG' $ii) $thisdir
    done
}

all()
{
    if ((clean == 1))
    then
        rm -f *.JPG *.png 
        rm -rf title cropped autocropped fused sprockets
        rm *.av? *.yuv 
    fi
    mkdir bw
    mkdir sprockets
    if ((0 != numbercheck))
    then
        exit 1
    fi
	precrop
#    sample
    echo autocrop
    autocrop
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

#oneAutocrop()
#{
#    autocrop.py -m super8 -s left -v -f $1 -o autocropped 2>&1
#    #autocrop.py -v -f $1 -o autocropped
#}
#
#export -f oneAutocrop

autocrop()
{
    echo Run autocrop
    if [[ ! -f "super8" && ! -f "8mm" ]]
    then
        echo No film type specified
        exit 1
    fi
	if ((clean == 1))
	then 
        rm -rf autocropped
	fi
    mkdir -p ./autocropped

    rm /tmp/autocrop_in_*.txt
    mode=""
    if [[ -f "8mm" ]]
    then
        mode="8mm"
    elif [[ -f "super8" ]]
    then
        mode="super8"
    fi
    if [[ "" == "$mode" ]]
    then
        echo "No autocrop mode set"
        return
    fi
    read -a croppedfiles <<< $(ls ${PROCDIR2}/cropped/*.JPG | sort)
    let numfiles=${#croppedfiles[@]}

    if ((($numfiles / 3) * 3 != ${#croppedfiles[@]}))
    then
        echo "Cropped file list must be a multiple of 3: $numfiles no good"
        return
    fi
    let numsections=$(nproc)
    let filespersection=$(((numfiles / 3) / numsections * 3))

    for ((section = 0; section < numsections; section++))
    do
        echo Section $section

        let startat=$((section * filespersection))
        let endat=$((startat + filespersection - 3))
        if ((numfiles - endat < filespersection))
        then
            let endat=$numfiles
        fi
        echo Section $section: $startat to $endat

        filename="/tmp/autocrop_in_${section}.txt"
        rm $filename
        for ((ii = startat; ii <= endat; ii += 3))
        do
            echo -n ${croppedfiles[$((ii + 0))]}"," >> $filename
            echo -n ${croppedfiles[$((ii + 1))]}",">> $filename
            echo ${croppedfiles[$((ii + 2))]} >> $filename
        done
    done

    for inputfile in /tmp/autocrop_in_*.txt
    do
        $SEM -N0 --jobs 200% autocrop.py --debug -m $mode  -v -l $inputfile -o ./autocropped 
    done

    $SEM --wait
}

autocropOld()
{
	if ((clean == 1))
	then 
        rm -rf autocropped
	fi
    mkdir autocropped
    mkdir sprockets
    mkdir bw

    read -a infiles <<< $(ls cropped/*.JPG)
    let numfiles=${#infiles[@]}

    for ((ii = 0; ii < numfiles - 3; ii += 3))
    do
        ifile1=${infiles[((ii + 0))]}
        ifile2=${infiles[((ii + 1))]}
        ifile3=${infiles[((ii + 2))]}

        if [[ -f autocropped/$(basename $ifile1) && \
              -f autocropped/$(basename $ifile2) && \
              -f autocropped/$(basename $ifile3) ]]
        then
            continue
        fi
        echo Autocropping ${file},${file2},${file3}
	    $SEM -N0 --jobs 200% autocrop.py -m super8 -s left \
        -v -f $ifile1,$ifile2,$ifile3 -o autocropped --debug
     done
     echo $SEM --wait
}


onePrecrop()
{
    infile=$1
	outfile=$2
    if [[ ! -f "$outfile" ]]
    then
    	convert $infile -crop ${3}x${4}+${5}+${6} -flop -flip $outfile
    fi
}

export -f onePrecrop

precrop()
{
	let numFrames=${1:-999999}
    if [[ -f crop.cfg ]]
    then
	    scaler
    fi
	if ((clean == 1))
	then
		rm -rf ${PROCDIR2}/cropped
	fi
 	mkdir -p ${PROCDIR2}/cropped

	let to=${TITLE_STREAM_FRAMES}
	((numFrames+=2))

	dirs=$(ls -d *PHOTO | sed 's/PHOTO//' | sort -n)

	for dir in $dirs
	do
		numbers=$(ls ${dir}PHOTO/SAM_*.JPG | xargs -I {} basename {} | cut -b5-8 | sort -n)
		if [[ -f ${dir}PHOTO/crop.cfg ]]
		then
			scaler ${dir}PHOTO/crop.cfg
		fi

		for number in $numbers
		do
			let index=$(echo -n 10#$to)
			outfile="${PROCDIR2}/cropped/SAM_$(printf '%06u' $to).JPG"
			if [[ ! -f $outfile ]]
			then
				filename=SAM_$(printf '%04u' $((10#$number))).JPG
				$SEM -N0 --jobs 200% onePrecrop ${dir}PHOTO/${filename} $outfile $width $height $xOffset $yOffset
				echo onePrecrop ${dir}PHOTO/${filename} $outfile $width $height $xOffset $yOffset
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
}

sample()
{
    sourcedir=$1
    targetdir=$2
    mkdir ${targetdir}
    let base=0
    for ((base = 0; ; base+=1000))
    do
        for ((offset = 200; offset < 210; offset++))
        do
            let number=$((base + offset))
            fromfile=${sourcedir}/SAM_$(printf '%06u' $((10#$number))).JPG
            tofile=${targetdir}/SAM_$(printf '%06u' $((10#$number))).JPG
            if [[ ! -f $fromfile ]]
            then
                return
            fi
            vOut sample $fromfile
            cp -u $fromfile $tofile
        done

    done
}


# optional for mac only it seems
optimize ()
{
	for ff in *PHOTO/*.JPG
	do
		$SEM -N0 --jobs 200% $IMAGE_OPTIM $ff
	done
}

#
# Functions resync, levelsMismatch, toneCheck
# - these routines check the levels.txt file generated by precrop to make sure that the cropped
# files are properly organized as exposure triples (ie, darkest, middle, brightest).
#
#declare -A explevels
#
#function resync()
#{
#	let base=$1
#	while ((${explevels[$base]} >= ${explevels[$((base+1))]}    
#		|| ${explevels[$((base+1))]} >= ${explevels[$((base+2))]}
#		|| 0 == ${explevels[$base]}
#		|| 0 == ${explevels[$((base+1))]}
#		|| 0 == ${explevels[$((base+2))]}))
#	do
#		((base++))
#	done
#	echo "Resynced at $base" >> $LEVELS_ERROR
#	echo $base
#}
#
#function explevelsMismatch()
#{
#	let base=$1
#	echo -n "Levels Mismatch at $base $((base+1)) $((base+2)) " >> $LEVELS_ERROR
#	echo "${explevels[$base]} ${explevels[$((base+1))]} ${explevels[$((base+2))]}" >> $LEVELS_ERROR
#	echo $(resync $base)
#}
#
#toneCheck()
#{
#	if [[ ! -f "$LEVELS_TXT" ]]
#	then
#		echo No $LEVELS_TXT file
#		exit 1
#	fi
#
#	rm $LEVELS_ERROR 2>/dev/null
#
#	for line in `seq 1 $(cat $LEVELS_TXT | wc -l)`
#	do
#		read index leveltxt <<< $(cat $LEVELS_TXT | sed -ne "${line}p;")
#		((index-=200))
#		let level=${leveltxt%%.*}
##		if ((0 == $level)); then echo -n '!'; fi
#		((explevels[$index] = $level))
#    done
#
#	let end=${#explevels[@]}
#	let ii=0
#	while (($ii < $end))
#	do
#		if (($ii + 3 >= $end))
#		then
#			echo "Not enough triples, terminating at $ii" >> $LEVELS_ERROR
#			exit 1
#		fi
#		let next=$((ii + 1))
#		let after=$((ii + 2))
#		if ((${explevels[$ii]} < ${explevels[$next]} && ${explevels[$next]} < ${explevels[$after]}))
#		then
#			((ii += 3))
#			continue
#		fi
#		let ii=$(explevelsMismatch $ii)
#	done
#}


tonefuse()
{
	let outindex=$TITLE_STREAM_FRAMES

	if ((clean == 1))
	then 
		rm -rf ${PROCDIR2}/fused
	fi
	mkdir -p ${PROCDIR2}/fused
	let baseindex=$TITLE_STREAM_FRAMES

    read -a infiles <<< $(ls autocropped/*.JPG)
    let numfiles=${#infiles[@]}

    for ((ii = 0; ii < numfiles - 3; ii += 3))
    do
		outfile="${PROCDIR2}/fused/SAM_$(printf "%06u" $((baseindex+3))).JPG"
        ((baseindex++))
        ifile1=${infiles[((ii + 0))]}
        ifile2=${infiles[((ii + 1))]}
        ifile3=${infiles[((ii + 2))]}

		if [[ ! -f $outfile ]]
		then
            echo tonefuse2: $ifile1,$ifile2,$ifile3 -> $outfile
			$SEM -N0 --jobs 200% TMPDIR=/home/mattd/tmp \
            enfuse --output $outfile $ifile1 $ifile2 $ifile3
		fi
     done
	$SEM --wait
    cd fused
    renumber 200 | sh
    cd ..
}

#oneToneFuseOld()
#{
#	dir=$1
#	baseindex=$2
#	outindex=$3
#	file1=SAM_$(printf "%06u" $baseindex)
#	file2=SAM_$(printf "%06u" $((baseindex+1)))
#	file3=SAM_$(printf "%06u" $((baseindex+2)))
#
#	outfile=fused/SAM_$(printf "%06u" $outindex).JPG
#	TMPDIR=/home/mattd/tmp enfuse --output $outfile ${dir}/${file1}.JPG ${dir}/${file2}.JPG ${dir}/${file3}.JPG
#}
#export -f oneToneFuse
#
#tonefuseOld()
#{
#	#scaler
#	let outindex=$TITLE_STREAM_FRAMES
#
#	if ((clean == 1))
#	then 
#		rm -rf fused
#	fi
#	if [[ ! -d fused ]]
#	then
#		mkdir fused
#	fi
#	let baseindex=$TITLE_STREAM_FRAMES
#
#	while [[ -f "autocropped/SAM_$(printf "%06u" $baseindex).JPG" ]]
#	do
#		vOut Tonefuse $outindex
#		outfile="fused/SAM_$(printf "%06u" $((baseindex+3))).JPG"
#		if [[ ! -f $outfile ]]
#		then
#			#sem -N0 --jobs 200% oneToneFuse $dir $baseindex $outindex
#			$SEM -N0 --jobs 200% oneToneFuse autocropped $baseindex $outindex
#		fi
#
#		((outindex++))
#		((baseindex+=3))
#	done
#	$SEM --wait
#}

oneCropFuse()
{
	dir=$1
	file1=$2
	file2=$3
	file3=$4
	outfile=$5
#	convert ${dir}PHOTO/${file1} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file1
#	convert ${dir}PHOTO/${file2} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file2
#	convert ${dir}PHOTO/${file3} -crop ${width}x${height}+${xOffset}+${yOffset} /tmp/$file3
    TMPDIR=/home/mattd/tmp enfuse --output $outfile ${dir}PHOTO/${file1} ${dir}PHOTO/${file2} ${dir}PHOTO/${file3}
#	rm /tmp/$file1 /tmp/$file2 /tmp/$file3
}
export -f oneCropFuse

cropfuse()
{
#	scaler
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
			#oneCropFuse $dir $file1 $file2 $file3 $outfile
		done
	done
	$SEM --wait
}

import()
{
    TARGET=${HD1}/`basename $PWD`
    mkdir -p $TARGET
	rsync -urav --ignore-existing ???PHOTO $TARGET/
	cp crop.cfg $TARGET
	cp title*.txt $TARGET
    cp super8 $TARGET
    cp 8mm $TARGET
}

pptests()
{
    let clean=1
    #for mode in cresultS{1,2,3,4,5,6} iresultS{1,2,3,4,5,6}
    for mode in cresultS7 iresultS7
    do
        postprocess $mode
    done
}


(date | tr -d '\n'; echo -n " "; pwd | tr -d '\n'; echo -n " ";  echo $*) >> $RUN_LOG

while getopts "f:msvC" OPT
do
    case $OPT in
        v) let verbose=1 ;;
        s) let simulate=1 ;;
		C) let clean=1 ;;
        f) let frameLimit=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

shift $((OPTIND-1))
case "$1" in 
	title) gentitle $2 ;;
	renumber) renumber $2 ;;
    preview) preview ;;
	previewtitle) previewTitle ;;
    genyuv) genyuv dvd;;
    mpeg2) mpeg2 web; mpeg2 dvd; mpeg2 hd ;;
	mp42jpg) mp42jpg $2 ;;
	avi) avi dvd ;;
	inter3|interpolate|[ic]compare|clean|bw) postprocess $1 ;;
	drange) deleterange $2 $3 ;;
	all) all ;;
	precrop) precrop ;;
    pcsample) sample cropped pcsample;; 
    tfsample) sample fused tfsample;; 
    autocrop) autocrop ;;
	optimize) optimize ;;
	tonefuse) tonefuse ;;
	cropfuse) cropfuse ;;
	import) import ;;
    pptests) pptests ;;
    numcheck) numbercheck ;;
    split) split ;;
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

