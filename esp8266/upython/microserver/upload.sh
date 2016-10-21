#!/bin/bash

#FILES="Server.py pg.htm boot.py"
#FILES="Server.py pg.htm boot.py"
FILES="Server.py pg.htm"

for ff in $FILES 
do
mpfshell -n -c  "open cu.SLAB_USBtoUART; put $ff"
sleep 2
done
