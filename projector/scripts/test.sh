#!/bin/bash

rawFrames=$(echo -n "Z:"; echo -n $PWD | sed 's/\//\\\\\\\\/g;'; echo "\\\\rawframes.avi")
echo "film=\""$rawFrames"\""
