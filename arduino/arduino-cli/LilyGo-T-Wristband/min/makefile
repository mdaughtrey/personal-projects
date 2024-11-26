LIBS=--libraries ../../../../libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
MAIN=min
#MYSKETCH=/personal-projects/arduino/arduino-cli/LilyGo-T-Wristband/libdeps/SparkFun_MPU-9250-DMP_Arduino_Library/examples/LilyGoWristband/MPU9250_Basic.ino
MYSKETCH=${PWD}/${MAIN}.ino

all: c u

c:
	arduino-cli --config-file ./arduino-cli.yaml --log-level trace --log-file ard.log compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload --input gesture/gesture.esp32.esp32.ttgo-t1 ${MYSKETCH} -b ${BOARD} -p ${PORT}
	#arduino-cli upload --input ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug ${MYSKETCH} -b ${BOARD} -p ${PORT}

s:
	screen /dev/ttyUSB0 115200

demo:
	arduino-cli upload --input examples/T-Wristband-MPU9250/T-Wristband-MPU9250.esp32.esp32.ttgo-t1.bin -b ${BOARD} -p ${PORT}

etags:
	find . -name '*.h' -exec etags -R -a  {} \;
	find . -name '*.cpp' -exec etags -R -a  {} \;
	find . -name '*.ino' -exec etags -R -a  {} \;
	find /root/.arduino15 '*.h' -exec etags -R -a  {} \;
