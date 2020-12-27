
TAGFILE=${PWD}/TAGS
PORT=/dev/ttyUSB0
#BOARD=esp32:esp32:esp32wrover
BOARD=esp32:esp32:esp32
MYSKETCH=bluemouth
#MYSKETCH=epd2in9d-demo
FLAGS=--log-file acli.log --log-level trace
# LIBS=--libraries ${PWD}/bluemouth/GxEPDEW029Z10LW
# Board JSON
# https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# T5 Ink Series
# https://github.com/Xinyuan-LilyGO/LilyGo-T5-ink-series
# GxEPD lib
# https://github.com/ZinggJM/GxEPD


all: c u

c:
	arduino-cli compile ${FLAGS} ${LIBS} -b ${BOARD} ${MYSKETCH}

u:
	arduino-cli upload ${MYSKETCH} -b ${BOARD} -p ${PORT}
	#arduino-cli upload --input ${MYSKETCH} -b ${BOARD} -p ${PORT}

d:
	arduino-cli debug -b ${BOARD} -p ${PORT} ${MYSKETCH}

s:
	screen /dev/ttyUSB0 115200

demo:
	arduino-cli upload --input examples/T-Wristband-MPU9250/T-Wristband-MPU9250.esp32.esp32.ttgo-t1.bin -b ${BOARD} -p ${PORT}

tags:
	etags -R -f ${TAGFILE} --exclude=.py . --langmap=C++:.ino
	cd ~/Arduino/libraries
	etags --append -f ${TAGFILE} --exclude=.py -R . --langmap=C++:.ino
	cd -

lib0:
	arduino-cli compile ${FLAGS} ${LIBS} -b ${BOARD} ~/Arduino/libraries/GxEPD-master/examples/GxEPD_Example
	arduino-cli upload ~/Arduino/libraries/GxEPD-master/examples/GxEPD_Example -b ${BOARD} -p ${PORT}

lib0u:
	arduino-cli upload ~/Arduino/libraries/GxEPD-master/examples/GxEPD_Example -b ${BOARD} -p ${PORT}
