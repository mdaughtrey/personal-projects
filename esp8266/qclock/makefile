PORT=/dev/ttyUSB0
BAUD=115200
UPFILE=qclock.py
IMAGE=esp8266-20200902-v1.13.bin
# https://micropython.org/download/esp8266/

screen:
	screen ${PORT} ${BAUD}
	#~/.local/bin/ampy --port ${PORT} put ${UPFILE}

put:
	~/.local/bin/ampy --port ${PORT} put ${UPFILE}
	~/.local/bin/ampy --port ${PORT} put boot.py

get:
	~/.local/bin/ampy --port ${PORT} --baud ${BAUD}  get boot.py > boot.py

erase:
	~/.local/bin/esptool.py --port ${PORT} erase_flash     

flashfirm:
	~/.local/bin/esptool.py --port ${PORT} --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
