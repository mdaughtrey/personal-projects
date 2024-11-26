#!/bin/bash

mydir=${PWD}
ctags -R
cd ~/.arduino15
ctags -R -o ${mydir}/tags
cd -

