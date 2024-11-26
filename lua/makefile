PROJ=rgbledstrip
PROJDIR=/personal-projects/arduino/rgbledstrip
PORT=/dev/ttyUSB0
#PORT=/dev/ttyS3
PLATFORM=esp8266:esp8266:generic
IMAGE=nodemcu-release-9-modules-2021-11-22-18-43-37-integer.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=~/acli/bin/arduino-cli
#MOUNT=/media/rp2040
GDB=~/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.0.4-gcc10.3-1757bed/bin/xtensa-lx106-elf-gdb
UPLOADER=nodemcu-uploader/nodemcu-uploader.py
DOCKERNAME=acli
#DOCKER=docker.exe
DOCKER=docker
FILES=main.lua

#ENVTYPE := $(or $(origin WSLENV), "linux")
ENVTYPE := $(or $(WSL_DISTRO_NAME), "linux")

all: b u s
	@echo Building	

# Build remote
br:
	ssh mattd@pop-os  ${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
        --entrypoint /home/mattd/acli/bin/arduino-cli \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}
	
# Build
b: 
	./build b
#	${DOCKER} run --rm -v $(LOCALVOL):/personal-projects \
#        --entrypoint /home/mattd/acli/bin/arduino-cli \
#		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}

# Upload remote
#ur: 
#	scp pop-os:~${PROJDIR}/${IMAGE} /tmp
#	sudo bossac -U --port=${PORT} -e -w /tmp/${IMAGE}

# Upload
u: 
	python3 ${UPLOADER} upload ${FILES}

a:
	docker attach acli

s:
	screen ${PORT} 115200

c:
	cat ${PORT} > serial.log

d:
	${GDB}

firmware:
	esptool.py --port /dev/ttyUSB0 write_flash -fm dio 0x00000 ${IMAGE}
#f:
#	sudo scp mattd@pop-os:~${PROJDIR}/${UF2} /media/rp2040/
#
#m:
#	sudo mount /dev/sdc1 /media/rp2040  -o rw,users
#
#mc: 	# mount and copy
#	./mntrp2040.sh
#	sudo cp  ${UF2} ${MOUNT}
#us:
#	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ~${PROJDIR}/${IMAGE}
#	screen ${PORT} 115200
