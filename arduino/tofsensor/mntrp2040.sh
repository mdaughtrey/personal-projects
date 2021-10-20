#!/usr/bin/bash

if [[ -f "/media/rp2040/INDEX.HTML" ]]; then exit 0; fi
read mnt<<<$(lsblk -JI 8 -o PATH,MAJ:MIN,MODEL,LABEL,PARTLABEL | jq '.blockdevices[] | select(.label == "RPI-RP2").path' | tr -d '"')
echo $mnt
if [[ "${mnt}" != "" ]]; then sudo mount $mnt /media/rp2040; exit 0; fi
exit 1
