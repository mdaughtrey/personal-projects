#!/bin/bash

threshold=${1:-0}

#set -o xtrace

if [[ ! -d blank ]]
then
	mkdir blank
else
	rm -f blank/*
fi

for ff in *.JPG
do
	if [[ $(stat --printf "%s" $ff) -lt $threshold ]]
	then
		 mv $ff blank
	fi
done
