LIBS=--libraries ./libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
#MYSKETCH=examples/T-Wristband-MPU9250

MYSKETCH=mine/wb.ino

c:
	arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload --input wb.ino.esp32.esp32.ttgo-t1.bin -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug ${MYSKETCH} -b ${BOARD} -p ${PORT}


		
