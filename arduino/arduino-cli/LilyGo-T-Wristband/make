#!/usr/bin/env bash

#LIBS='--libraries ./libdeps/PCF8563_Library,./libdeps/TFT_eSPI'
LIBS='--libraries ./libdeps/'
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:tinypico
BIN=smartband.bin
MYSKETCH=examples/T-Wristband-MPU9250

#/T-Wristband-MPU9250.ino.esp32.esp32.tinypico.bin

#url=https://github.com/lewisxhe/PCF8563_Library

case "$1" in 
	u) arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT} ;;
	*) arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH} ;;
	#*) arduino-cli compile ${LIBS} -b ${BOARD} examples/T-Wristband-MPU9250/T-Wristband-MPU9250.ino ;;
esac
