#!/bin/bash

ERRORFILE="./errorcheck.out"
SEM="sem --will-cite -N0 --jobs 200%"
declare -A levels

if [[ -f "$ERRORFILE" ]]
then
	rm $ERRORFILE
fi

function getLevel()
{
	let index=$(echo -n '10#'; echo $1 | cut -c5-10)
	if [[ ! -f "$1" ]]
	then
		echo "$1 Missing or corrupted" >> $2
		echo $index 0
	fi
	level=$(identify -format '%[mean]' $1 2>>$2) 
	if [[ "$?" -ne "0" ]]
	then
		echo $index 0 
		return
	fi
	echo $index ${level%%.*}
}

export -f getLevel

function resync()
{
	let base=$1
	while ((${levels[$base]} >= ${levels[$((base+1))]}    
		|| ${levels[$((base+1))]} >= ${levels[$((base+2))]}
		|| 0 == ${levels[$base]}
		|| 0 == ${levels[$((base+1))]}
		|| 0 == ${levels[$((base+2))]}))
	do
		((base++))
	done
	echo "Resynced at $base" >> $ERRORFILE
	echo $base
}

function levelsMismatch()
{
	let base=$1
	echo -n "Levels Mismatch at $base $((base+1)) $((base+2)):" >> $ERRORFILE
	echo "${levels[$base]} ${levels[$((base+1))]} ${levels[$((base+2))]}" >> $ERRORFILE
	echo $(resync $base)
}

for ff in $@
do
	read index level <<< $($SEM getLevel $ff $ERRORFILE) 
	if ((0 == $level))
	then echo -n ! 
	else echo -n .
	fi
	let levels[$index]=$level
done

let end=${#levels[@]}
((end+=200))
let ii=200
while (($ii < $end))
do
	if (($ii + 3 >= $end))
	then
		echo "Not enough triples, terminating at $ii" >> $ERRORFILE
		exit 1
	fi
	let next=$((ii + 1))
	let after=$((ii + 2))
	if ((${levels[$ii]} < ${levels[$next]} && ${levels[$next]} < ${levels[$after]}))
	then
		((ii += 3))
		continue
	fi
	let ii=$(levelsMismatch $ii)
done
echo

if [[ -f "$ERRORFILE" ]]
then	
	exit 1
fi
exit 0


