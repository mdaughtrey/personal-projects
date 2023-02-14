#!/bin/bash

NAME=avxsynth
ARGS=" -i /frames/test.avs /frames/out.mp4"
docker run --rm -h ${NAME} --name ${NAME} -ti --entrypoint /ffmpeg/ffmpeg -v ${HOME}/share:/frames -v ${PWD}:/local ${NAME}:latest ${ARGS}
