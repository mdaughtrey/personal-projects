LIBS=--libraries ./libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
MYSKETCH=examples/T-Wristband-MPU9250
#MYSKETCH=examples/T-Wristband-LSM9DS1/
#MYSKETCH=examples/T-Wristband-MAX3010X/
#MYSKETCH=blinky

c:
	arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug ${MYSKETCH} -b ${BOARD} -p ${PORT}


		
