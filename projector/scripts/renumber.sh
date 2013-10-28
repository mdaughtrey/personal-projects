#!/bin/bash

#set -o xtrace

let copyto=0
let copyfrom=0
let count=0
let copycount=$(ls SAM_*.JPG | wc -l)

while [[ ! -f SAM_`printf '%04u' $copyfrom`.JPG ]]
do
	((copyfrom++))
done

while ((count < copycount))
do
	if [[ -f SAM_`printf '%04u' $copyfrom`.JPG ]]
	then
		echo mv SAM_`printf '%04u' $copyfrom`.JPG SAM_`printf '%04u' $copyto`.JPG 
		((copyto++))
	fi
	((count++))
	((copyfrom++))
done


