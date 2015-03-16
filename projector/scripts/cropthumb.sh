#!/bin/bash

CTDIR=cropthumbs
HTMLFILE=./cropthumbs.html

if [[ ! -d "$CTDIR" ]]
then
		mkdir $CTDIR
fi

doCrop()
{
files=`ls cropped/SAM_*.JPG`
for file in $files
do
		outfile=`basename $file`
		echo $outfile
		if [[ ! -f "$CTDIR/$outfile" ]]
		then
				convert -resize 100x $file $CTDIR/$outfile
		fi
done
}

doMontage()
{
let count=0
while [[ 0 ]]
do
		outfile=$(printf "$CTDIR/montage%d.JPG" $count)
		echo $outfile
		if [[ -f "$outfile" ]]
		then
			((count++))
			continue
		fi
		filetemplate=$(printf "$CTDIR/SAM_%03u*.JPG" $count)
		let numfiles=$(ls $filetemplate | wc -l)
		if (( 0 == $numfiles ))
		then
			break
		fi

		montage $filetemplate -geometry +0+0 -tile 18x $outfile
		((count++))
done
}

doHtml()
{
echo "<html><head></head><body><table>" > $HTMLFILE

files=`ls -lrt cropthumbs/montage*.JPG`

let count=0

for file in $files
do
		echo -n "<tr><td><img src='"$file"' alt='"$file"'></td></tr>" >> $HTMLFILE
done
echo "</table></body>" >> $HTMLFILE
}



case "$1" in
	montage) doMontage ;;
	html) doHtml ;;
esac
