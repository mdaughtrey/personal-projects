#!/bin/bash
PORT=tty.SLAB_USBtoUART
esptool.py --port /dev/$PORT --baud 115200 write_flash --flash_size=8m 0 esp8266-20160909-v1.8.4.bin 
