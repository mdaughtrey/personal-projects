LIBS=--libraries ../libdeps/
PORT=/dev/ttyUSB0
BOARD=esp32:esp32:ttgo-t1
MYSKETCH=${PWD}/gesture.ino

#BP=--build-properties build.extra_flags=-DLOAD_FONT2 
#   --build-properties build.extra_flags=-DLOAD_FONT4
#BP=--build-properties build.extra_flags=-DLOAD_GFXFF

all: c u


c:
	arduino-cli compile ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT}
	#arduino-cli upload --input ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug -b ${BOARD} -p ${PORT} ${MYSKETCH}

s:
	screen /dev/ttyUSB0 115200

demo:
	arduino-cli upload --input examples/T-Wristband-MPU9250/T-Wristband-MPU9250.esp32.esp32.ttgo-t1.bin -b ${BOARD} -p ${PORT}

etags:
	find . -name '*.h' -exec etags -R -a  {} \;
	find . -name '*.cpp' -exec etags -R -a  {} \;
	find . -name '*.c' -exec etags -R -a  {} \;
	find . -name '*.ino' -exec etags -R -a  {} \;
	find /root/.arduino15 -name '*.h' -exec etags -R -a  {} \;
