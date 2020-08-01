LIBS=--libraries /personal-projects/arduino/arduino-cli/LilyGo-T-Wristband/libdeps/

PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
MYSKETCH=.

c:
	arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug ${MYSKETCH} -b ${BOARD} -p ${PORT}


		
