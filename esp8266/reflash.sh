#!/bin/bash
esptool.py --port /dev/cu.SLAB_USBtoUART --baud 460800 write_flash --flash_size=8m 0 esp8266-20160909-v1.8.4.bin 
