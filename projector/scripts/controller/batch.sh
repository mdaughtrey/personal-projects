#!/bin/bash


#titles=opto2
titles=niall004
for t in ${titles}; do
	PROJECT=${t} ./runhq.sh import
	PROJECT=${t} ./runhq.sh pngcrop
	PROJECT=${t} ./runhq.sh peek
	PROJECT=${t} ./runhq.sh peekcb
#	PROJECT=${t} ./runhq.sh processavi
done

