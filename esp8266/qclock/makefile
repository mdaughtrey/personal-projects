PORT=/dev/ttyUSB0
BAUD=115200
UPFILE=qclock.py

screen:
	screen ${PORT} ${BAUD}
	#~/.local/bin/ampy --port ${PORT} put ${UPFILE}

put:
	~/.local/bin/ampy --port ${PORT} put ${UPFILE}

get:
	~/.local/bin/ampy --port ${PORT} get boot.py > boot.py

erase:
	~/.local/bin/esptool.py --port ${PORT} erase_flash     

flashfirm:
	~/.local/bin/esptool.py --port ${PORT}--baud 460800 write_flash --flash_size=detect 0 esp8266-20191220-v1.12.bin
