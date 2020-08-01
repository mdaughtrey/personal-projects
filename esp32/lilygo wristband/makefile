PORT=/dev/ttyUSB0
BAUD=115200
UPFILES=$(wildcard bluetooth/*.py) boot.py

put:
	$(foreach file,$(UPFILES), $(shell ~/.local/bin/ampy --port $(PORT) put $(file)))

get:
	~/.local/bin/ampy --port ${PORT} --baud ${BAUD}  get boot.py > boot.py

firmware:
	esptool.py --chip esp32 --port ${PORT} erase_flash
	esptool.py --chip esp32 --port ${PORT} --baud 460800 write_flash -z 0x1000 tinypico-idf4-20200316-v1.12.bin

screen:
	screen /dev/ttyUSB0 115200
