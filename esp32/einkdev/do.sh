
erase:
~/.local/bin/esptool.py --port /dev/ttyUSB0 erase_flash     

flashfirm:
~/.local/bin/esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash --flash_size=detect 0 esp8266-20191220-v1.12.bin

~.local/bin/ampy

