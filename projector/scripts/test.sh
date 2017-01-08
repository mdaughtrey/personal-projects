#!/bin/bash

FONT="/usr/share/fonts/truetype/droid/DroidSerif-BoldItalic.ttf"
SEM="sem --will-cite"
convert -background transparent -stroke yellow -strokewidth 2 \
-fill blue -font $FONT -size x200 \
'label:Sample Title' \
/media/sf_vmshared/scans//blister/title/titleline_0.png
