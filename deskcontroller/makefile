PORT=/dev/ttyUSB0
BAUD=115200
UPFILE=deskcontroller.py
IMAGE=esp8266-20210618-v1.16.bin
# https://micropython.org/download/esp8266/
APDS9960_DIR=apds9960-0.2/apds9960
APDS9960=const.py device.py exceptions.py 

screen:
	screen ${PORT} ${BAUD}
	#~/.local/bin/ampy --port ${PORT} put ${UPFILE}

pm:
	ampy --port ${PORT} put ${UPFILE}

put:
	ampy --port ${PORT} put ${UPFILE}
	ampy --port ${PORT} put boot.py
	$(foreach ff, $(APDS9960), echo ${ff} ; ampy --port ${PORT} put ${APDS9960_DIR}/${ff}; )

get:
	ampy --port ${PORT} --baud ${BAUD}  get boot.py > boot.py

erase:
	esptool --port ${PORT} erase_flash     

flashfirm:
	esptool --port ${PORT} --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
