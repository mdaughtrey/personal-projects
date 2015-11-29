#!/bin/bash

if [[ "$1" == "" ]]
then
	echo Need target index
	exit 1
fi
let target=$1

ls -d ???PHOTO | while read from
do
	to=`printf '%03uPHOTO' $target`
	if [[ -e "$to" ]]
	then
		$to already exists
		exit 1
	fi
	echo mv $from $to
	((target++))
done

	
