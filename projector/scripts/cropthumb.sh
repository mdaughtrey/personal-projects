#!/bin/bash

CTDIR=cropthumbs
HTMLFILE=./cropthumbs.html
LEVELFILE=${1:-levelcheck.out}

if [[ ! -d "$CTDIR" ]]
then
		mkdir $CTDIR
fi

makeThumb()
{
	infile=$(printf "cropped/SAM_%06u.JPG" $1)
	outfile=$(printf "${CTDIR}/SAM_%06u.JPG" $1)

	if [[ ! -f "$outfile" ]]
	then
		convert -resize 100x $infile $outfile
	fi
}

makeMontage()
{
	let mcount=0
	montageFile=$1
	shift
    filelist=""
	for ii in $@
	do
		filelist=$(printf "$filelist ${CTDIR}/SAM_%06u.JPG" $ii)
		((count++))
	done
	montage $filelist -geometry +1+1 -tile ${count}x $montageFile
}

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

echo "<html><head></head><body bgcolor='#a0a0ff'><table>" > $HTMLFILE

#files=`ls -lrt cropthumbs/montage*.JPG`

let count=0
#cat $LEVELFILE | grep 'Levels Mismatch' | awk -F' ' '{print $4,$6}' \
# | while read iOne iTwo
cat $LEVELFILE | grep 'Levels Mismatch' | while read line
do
	read iOne iTwo <<< $(echo $line | awk -F' ' '{print $4,$6}')

	let first=$iOne
	let last=$iTwo
	filelist=""
	((first-=10))
    ((last+=10))
	echo Montage $first $last $count
	for ii in `seq $first $last`
    do
		filelist="$filelist $ii"
		makeThumb $ii
	done

	montageFile=$(printf "${CTDIR}/montage%d.JPG" $count)
	makeMontage $montageFile $filelist
	echo -n "<tr><td>${line}</td></tr>" >> $HTMLFILE
	echo -n "<tr><td><img src='"$montageFile"' alt='"$filelist"'></td></tr>" >> $HTMLFILE
	((count++))
done
echo "</table></body>" >> $HTMLFILE

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
