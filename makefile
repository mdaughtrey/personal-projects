LIBS=--libraries ./libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
#MYSKETCH=examples/T-Wristband-MPU9250
BP=--build-properties build.extra_flags=-DLOAD_FONT2 
#   --build-properties build.extra_flags=-DLOAD_FONT4

MYSKETCH=gesture

c:
	arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH}

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
