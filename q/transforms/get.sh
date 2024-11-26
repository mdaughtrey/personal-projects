#!/bin/bash

#URL="http://code.kx.com/q4m3/"
URL="https://en.wikipedia.org/wiki/Transformation_matrix"
#-N -r -l inf --no-remove-listing

#wget --execute="robots = off" --mirror --convert-links --no-parent --wait=1 \
#wget --execute="robots = off" --mirror -l 1 --convert-links --no-parent  \
wget --execute="robots = off" --convert-links --no-parent  \
    $URL
