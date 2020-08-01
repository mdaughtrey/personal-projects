LIBS=--libraries ../../../../libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
#MYSKETCH=/personal-projects/arduino/arduino-cli/LilyGo-T-Wristband/libdeps/SparkFun_MPU-9250-DMP_Arduino_Library/examples/LilyGoWristband/MPU9250_Basic.ino
MAIN=MPU9250_Basic
MYSKETCH=${PWD}/${MAIN}

all: c u

c:
	arduino-cli --config-file arduino-cli.yaml compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT}
	#arduino-cli upload --input ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug ${MYSKETCH} -b ${BOARD} -p ${PORT}

s:
	screen /dev/ttyUSB0 115200

demo:
	arduino-cli upload --input examples/T-Wristband-MPU9250/T-Wristband-MPU9250.esp32.esp32.ttgo-t1.bin -b ${BOARD} -p ${PORT}

etags:
	ctags -R
