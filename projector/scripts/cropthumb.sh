#!/bin/bash

CTDIR=cropthumbs
HTMLFILE=./cropthumbs.html

if [[ ! -d "$CTDIR" ]]
then
		mkdir $CTDIR
fi

echo "<html><head></head><body><table>" > $HTMLFILE

files=`ls cropped/SAM_*.JPG`

let count=0

for file in $files
do
		if ((count % 12 == 0))
		then
				echo -n "<tr>" >> $HTMLFILE
		fi
		outfile=`basename $file`
		echo $outfile
		if [[ ! -f "$CTDIR/$outfile" ]]
		then
				convert -resize 100x $file $CTDIR/$outfile
		fi
		echo -n "<td><img src='"$CTDIR/$outfile"' alt='"$outfile"'></td>" >> $HTMLFILE

		if ((count % 12 == 11))
		then
				echo "</tr>" >> $HTMLFILE
		fi
		((count++))
done

echo "</table></body>" >> $HTMLFILE
