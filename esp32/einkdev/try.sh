#!/bin/bash

PORT=/dev/ttyUSB0
#BAUD=256000
BAUD=115200
IMAGE=micropython.org/resources/firmware/esp32-idf4-20191220-v1.12.bin

#for image in `find . -name '*.bin'`
#do
#    echo $image | tee -a try.log
	~/.local/bin/esptool.py --port ${PORT} erase_flash     
	~/.local/bin/esptool.py --chip esp32 --port ${PORT} --baud 460800 write_flash --flash_size=detect 0x1000 ${IMAGE}
#	screen ${PORT} ${BAUD}
#    read
#done
