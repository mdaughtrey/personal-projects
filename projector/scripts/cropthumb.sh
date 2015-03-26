#!/bin/bash

OUTPUTDIR=croppedthumbs
export INPUTDIR=cropped
export TMPDIR=/tmp/cropthumbs
let MONTAGECOLS=18
let MONTAGEROWS=20
HTMLFILE=${OUTPUTDIR}/cropthumbs.html
LEVELFILE=${1:-levelcheck.out}
SEM="sem --will-cite -N0 --jobs 200%"

mkdir $OUTPUTDIR
mkdir $TMPDIR
rm $TMPDIR/*

makeThumb()
{
	infile=$(printf "${INPUTDIR}/SAM_%06u.JPG" $1)
	outfile=$(printf "${TMPDIR}/SAM_%06u.JPG" $1)

	if [[ ! -f "$outfile" ]]
	then
		convert -resize 100x $infile $outfile
	fi
}
export -f makeThumb

makeMontage()
{
	let base=$1
	let counttil=$((MONTAGEROWS * MONTAGECOLS + 200))
	montageFile=$2
    filelist=""

	let count=0
	while (($count < $counttil))
	do
		filelist=$(printf "$filelist ${TMPDIR}/SAM_%06u.JPG" $((base + MONTAGEROWS * MONTAGECOLS)))
		((count++))
	done
	montagefile=$(printf "${OUTPUTDIR}/montage_%06u_%06u.JPG" $base $((base + counttil)))
#	makeMontage $((count + 200)) # ${OUTPUTDIR}/$(printf "montage_%06u.JPG" $montage)
	montage $filelist -geometry +1+1 -tile ${MONTAGECOLS}x $montagefile
	rm $filelist
}

let numfiles=$(ls ${INPUTDIR}/SAM_*.JPG | wc -l)
let montage=0
let count=0

set -o xtrace

while ((count < numfiles))
do
	let thcount=0
	while (($thcount < $MONTAGEROWS * $MONTAGECOLS))
	do
		$SEM makeThumb $((200 + count + thcount)) # ${OUTPUTDIR}/$(printf "montage_%06u.JPG" $montage)
		((thcount++))
    done
	makeMontage $((count + 200)) # ${OUTPUTDIR}/$(printf "montage_%06u.JPG" $montage)
	((count += $MONTAGEROWS * $MONTAGECOLS))
	((montage++))
done

#echo "<html><head></head><body><table>" > $HTMLFILE
#doCrop()
#{
#files=`ls cropped/SAM_*.JPG`
#for file in $files
#do
#	outfile=`basename $file`
#	echo $outfile
#	if [[ ! -f "$CTDIR/$outfile" ]]
#	then
#		convert -resize 100x $file $CTDIR/$outfile
#	fi
#done
#}

#doMontage()
#{
#let count=0
#while [[ 0 ]]
#do
#		outfile=$(printf "$CTDIR/montage%d.JPG" $count)
#		echo $outfile
#		if [[ -f "$outfile" ]]
#		then
#			((count++))
#			continue
#		fi
#		filetemplate=$(printf "$CTDIR/SAM_%03u*.JPG" $count)
#		let numfiles=$(ls $filetemplate | wc -l)
#		if (( 0 == $numfiles ))
#		then
#			break
#		fi
#
#		montage $filetemplate -geometry +0+0 -tile 18x $outfile
#		((count++))
#done
#}

#echo "<html><head></head><body bgcolor='#a0a0ff'><table>" > $HTMLFILE

#files=`ls -lrt cropthumbs/montage*.JPG`

#let count=0
#cat $LEVELFILE | grep 'Levels Mismatch' | awk -F' ' '{print $4,$6}' \
# | while read iOne iTwo
#do
##	read iOne iTwo <<< $(echo $line | awk -F' ' '{print $4,$6}')
#
#	let first=$iOne
#	let last=$iTwo
#	filelist=""
#	((first-=3))
#    	((last+=3))
##	echo Montage $first $last #$count
#	for ii in `seq $first $last`
#        do
#		cp cropped/SAM_$(printf '%06u' $ii).JPG $CTDIR
##		filelist="$filelist $ii"
##		makeThumb $ii
#	done
#
##	montageFile=$(printf "${CTDIR}/montage%d.JPG" $count)
##	makeMontage $montageFile $filelist
##	echo -n "<tr><td>${line}</td></tr>" >> $HTMLFILE
##	echo -n "<tr><td><img src='"$montageFile"' alt='"$filelist"'></td></tr>" >> $HTMLFILE
##	((count++))
#done
##echo "</table></body>" >> $HTMLFILE

#let count=0
#while [[ 0 ]]
#do
#		outfile=$(printf "$CTDIR/montage%d.JPG" $count)
#		echo $outfile
#		if [[ -f "$outfile" ]]
#		then
#			((count++))
#			continue
#		fi
#		filetemplate=$(printf "$CTDIR/SAM_%03u*.JPG" $count)
#		let numfiles=$(ls $filetemplate | wc -l)
#		if (( 0 == $numfiles ))
#		then
#			break
#		fi
#
#		montage $filetemplate -tile 18x $outfile
#		((count++))
#done

#echo "<html><head></head><body><table>" > $HTMLFILE
#
#files=`ls cropthumbs/montage*.JPG`
#
#let count=0
#
#for file in $files
#do
#		echo -n "<tr><td><img src='"$CTDIR/$outfile"' alt='"$outfile"'></td></tr>" >> $HTMLFILE
#done
#echo "</table></body>" >> $HTMLFILE
#		echo -n "<tr><td><img src='"$file"' alt='"$file"'></td></tr>" >> $HTMLFILE
#done
#echo "</table></body>" >> $HTMLFILE
#}
#
#
#
#case "$1" in
#	montage) doMontage ;;
#	html) doHtml ;;
#esac
