#!/bin/bash


if [[ "$1" == "" ]]
then
    echo Project name?
    exit 1
fi

mkdir $1
cp makefile $1
cp cintemp.cpp $1/main.cpp
cp .gdbinit $1
cd $1
make
