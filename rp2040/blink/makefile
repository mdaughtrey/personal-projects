PROJ=blink
PROJDIR=/personal-projects/rp2040/blink
PORT=/dev/ttyACM0
PLATFORM=rp2040:rp2040:generic
UF2=${PROJ}.ino.uf2
BIN=${PROJ}.ino.bin
ACLI=~/acli/bin/arduino-cli
MOUNT=/media/rp2040
DOCKERNAME=acli
DOCKER=docker

#lsblk -I 8 -o PATH,MAJ:MIN,MODEL,LABEL,PARTLABEL

#b:
#	${ACLI} compile ${PROJ}.ino -b ${PLATFORM} --output-dir . 
	#${ACLI} compile ${PROJ}.ino -b ${PLATFORM} --output-dir . --build-property build.extra_flags=-D__AVR__=1

#u:
#	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
# Build
b:
	${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile ${PROJDIR}/${PROJ}.ino -b esp8266:esp8266:generic --output-dir  ${PROJDIR}

a:
	docker attach acli

s:
	screen ${PORT} 115200

u:
#	stty -F /dev/ttyACM0 1200
	bossac --port=${PORT} --usb-port=true -e -a  -w -v  ${BIN}

mc: 	# mount and copy
	./mntrp2040.sh
	sudo cp  ${UF2} ${MOUNT}

us:
	- bossac --port=${PORT} --usb-port=true  -e -a  -w -v  ${IMAGE}
	./mntrp2040.sh
	sudo cp  ${UF2} ${MOUNT}
#	while [ ! -d "/media/mattd/RPI-RP2" ]; do echo -n "."; sleep 1; done
##	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
#	screen ${PORT} 115200
