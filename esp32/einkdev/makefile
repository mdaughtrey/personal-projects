PORT=/dev/ttyUSB0
BAUD=115200
UPFILE=main.py
ALLFILES=main.py boot.py micropython-waveshare-epaper/epaper2in13.py ssd1675a.py
#IMAGE=esp32spiram-idf4-20191220-v1.12.bin
#IMAGE=tinypico-idf4-20191220-v1.12.bin
IMAGE=esp32-idf4-20191220-v1.12.bin
#IMAGE=esp32-idf3-20180511-v1.9.4.bin

s:
	screen ${PORT} ${BAUD}
	#~/.local/bin/ampy --port ${PORT} put ${UPFILE}

p:
	$(foreach ff, $(ALLFILES), echo ${ff} ; ~/.local/bin/ampy --port ${PORT} put ${ff}; )

pm:
	~/.local/bin/ampy --port ${PORT} put ${UPFILE}

g:
	~/.local/bin/ampy --port ${PORT} --baud ${BAUD}  get boot.py > boot.py

e:
	~/.local/bin/esptool.py --port ${PORT} erase_flash     

f:
	~/.local/bin/esptool.py --port ${PORT} erase_flash     
	~/.local/bin/esptool.py --chip esp32 --port ${PORT} --baud 460800 write_flash --flash_size=detect 0x1000 ${IMAGE}

cid:
	~/.local/bin/esptool.py --chip esp32 --port ${PORT} --baud 460800 chip_id
