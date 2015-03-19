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
	i2=$(printf "cropped/SAM_%06u.JPG" $2)
	i3=$(printf "cropped/SAM_%06u.JPG" $3)
	i4=$(printf "cropped/SAM_%06u.JPG" $4)
#	outfile=printf( "${CTDIR}/montage%d.JPG" $1)
	montage $i2 $i3 $i4 -tile 18x $1
}

echo "<html><head></head><body><table>" > $HTMLFILE

let count=0
cat $LEVELFILE | grep 'Levels Mismatch' | awk -F' ' '{print $4,$5,$6}' \
 | while read iOne iTwo iThree
do
	echo Montage $iOne $iTwo $iThree $count
	makeThumb $iOne
	makeThumb $iTwo
	makeThumb $iThree
	montageFile=$(printf "${CTDIR}/montage%d.JPG" $count)
	makeMontage $montageFile $iOne $iTwo $iThree
	echo -n "<tr><td><img src='"$montageFile"' alt='"$iOne $iTwo $iThree"'></td></tr>" >> $HTMLFILE
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
