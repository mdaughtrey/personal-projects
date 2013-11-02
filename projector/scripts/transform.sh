#!/bin/bash

#set -o xtrace
exec > $0.log 2>&1

date

params=( $( head -1 transform.cfg ) )

let beforeULx=${params[0]}
let beforeULy=${params[1]}
let beforeURx=${params[2]}
let beforeURy=${params[3]}
let beforeLRx=${params[4]}
let beforeLRy=${params[5]}
let beforeLLx=${params[6]}
let beforeLLy=${params[7]}

let afterLeft=${params[8]}
let afterTop=${params[9]}
let afterRight=${params[10]}
let afterBottom=${params[11]}

let afterULx=$afterLeft
let afterULy=$afterTop
let afterURx=$afterRight
let afterURy=$afterTop
let afterLRx=$afterRight
let afterLRy=$afterBottom
let afterLLx=$afterLeft
let afterLLy=$afterBottom

let cropWidth="$afterLRx - $afterLLx"
let cropHeight="$afterLRy - $afterURy"
let cropXOffset=$afterULx
let cropYOffset=$afterULy

echo Counting files... | tr -d '\n'
let numFiles=$(ls *.JPG | wc -l)
echo $numFiles
let numWorkers=${params[12]}

function doJob
{
let from=$1
let to=$2
let index=$from
echo I will process $from to $to

while ((index <= to))
do
	file=SAM_`printf '%04d' $index`.JPG
	echo [$from/$to] Processing $index
	if [[ ! -f $file ]]
	then
		echo File $file does not exist, ending
 		break
	fi
	convert $file -matte -virtual-pixel black -distort BilinearForward  "\
		$beforeULx,$beforeULy $afterULx,$afterULy \
		$beforeURx,$beforeURy $afterURx,$afterURy \
		$beforeLRx,$beforeLRy $afterLRx,$afterLRy \
		$beforeLLx,$beforeLLy $afterLLx,$afterLLy"\
		 -crop "${cropWidth}x${cropHeight}+${cropXOffset}+${cropYOffset}" \
		transformed/`basename $file`
((index++))
done
}

if [[ ! -d "transformed" ]]
then
	echo Creating transformed directory
	mkdir transformed
else
	echo Cleaning out transformed directory
	rm  transformed/*
fi 

let chunk=$((numFiles/numWorkers))
let offset=0

while ((offset <= numFiles))
do
	doJob $offset $((offset + chunk - 1)) &
	((offset += chunk))
done

for job in `jobs -p`
do
	echo Waiting on $job
	wait $job
done
date
