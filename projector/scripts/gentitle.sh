#!/bin/bash

set -o xtrace
declare -ix TITLE_STREAM_FRAMES=200
#FONT="/usr/share/fonts/truetype/droid/DroidSerif-BoldItalic.ttf"
SOFTWARE=/media/sf_vproj/scans/software/
FONT=${SOFTWARE}/fonts/DroidSerif-BoldItalic.ttf
SEM=sem
use="no"

while getopts "p:r:u:" OPT
do
    case $OPT in
        p) project=$OPTARG ;;
        r) fileroot=$OPTARG ;;
        u) use=$OPTARG ;;
        *) echo What?; exit 1 ;;
    esac
done

export imgroot=$fileroot/$project
export titleroot=$imgroot/title

if [[ "" == $project || "" == $fileroot ]]
then
    echo Some parameter is missing
    exit 1
fi

getFirstImage()
{
    case $use in
    ac)
        IFS=\|
        select="select container,autocrop from picdata limit 1"
        sqlite3 -list $fileroot/${project}/${project}db "$select" | while read container filename 
        do
            echo $fileroot/$project/$container/autocrop/${filename}a".jpg"
        done ;;

    *)
        IFS=\|
        select="select container,fused from picdata limit 1"
        sqlite3 -list $fileroot/${project}/${project}db "$select" | while read container filename tag
        do
            echo $fileroot/$project/$container/fused/${filename}".jpg"
        done ;;
    esac
}

oneTitleFrame()
{
	index=$1
    inputfile=$2
	translateX=$3
	translateY=$4
	titlefile=$5
    titleroot=$6

	inc=$(echo "scale=1;100/${TITLE_STREAM_FRAMES}" | bc -l)
	value=$(echo "${inc}*${index}" | bc -l)
    #underlay=$titleroot/underlay$(printf '%06u' $index).png
    underlay=/tmp/underlay$$.png
	eval "convert $inputfile -blur 2x2 -modulate 100,${value} $underlay"
	convert $underlay -page +${translateX}+${translateY} \
		 $titlefile -layers merge $titleroot/title$(printf '%06u' $index).JPG
    rm $underlay
}

export -f oneTitleFrame

#$renderIndex $firstfile $translateX $translateY $titleroot/titletext${pageIndex}.png $titleroot

gentitles()
{
    read -a images <<<$(getFirstImage)

    if [[ ! -d $titleroot ]]
    then
	    mkdir $titleroot
    fi

	let rowsize=200
	let linecount=0
	let translateX=0
	let translateY=0
	let pageIndex=0
     # -rotate -90 $titleroot/titleline_${linecount}.png

	for pageFile in $(ls $imgroot/title?.txt)
	do
#		echo Generate Title Page $pageIndex
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
				-size x${rowsize} label:"${line}" \
                $titleroot/titleline_${linecount}.png
			((translateY+=rowsize+20))
			#echo -n " -page +$((translateX))+$((translateY)) $titleroot/titleline_${linecount}.png"
			echo -n " -page +0+$((translateY)) $titleroot/titleline_${linecount}.png"
			((linecount++))
		done) 

		compositecmd="convert ${compositecmd} -background transparent -layers merge $titleroot/titletext${pageIndex}.png"
		#echo $compositecmd
		$compositecmd
		#rm $titleroot/titleline_*.png
		((pageIndex++))
    done	

	# get the underlay geometry
    firstfile=${images[0]}
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
		geometry=($(identify -ping $titleroot/titletext${pageIndex}.png  | cut -d' ' -f3 | sed 's/x/ /g'))
		let titleW=${geometry[0]}
		let titleH=${geometry[1]} 
		let translateX=$(($underlayW/2 - $titleW/2))
		let translateY=$(($underlayH/2 - $titleH/2))
		for renderIndex in `seq $renderStart $renderEnd`
		do
            echo Title $renderIndex
			$SEM -N0 --jobs 200% oneTitleFrame $renderIndex $firstfile $translateX $translateY $titleroot/titletext${pageIndex}.png $titleroot
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
	sem --wait
}

genyuvstream()
{
    if [[ "200" != `ls $titleroot/title??????.JPG | wc -l` ]]
    then
        gentitles
    fi
    ls $titleroot/title*.JPG | sort -n > $titleroot/titlelist.txt
	options="-quiet -mf fps=18 -benchmark -nosound -noframedrop -noautosub -vo yuv4mpeg" 
    cropoptions="-vf scale=1920:1080"
   	mplayer -lavdopts threads=`nproc` mf://@$titleroot/titlelist.txt $cropoptions ${options}:file=$titleroot/titles.yuv
}

converttoavi()
{
    if [[ ! -f "$titleroot/titles.yuv" ]]
    then
        genyuvstream
    fi
    cat $titleroot/titles.yuv | yuvfps -v 0 -r 18:1 -v 1 | avconv -loglevel info -i - -vcodec rawvideo -y $titleroot/titles.avi
}

gentitles
