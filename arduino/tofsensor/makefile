PROJ=tofsensor
PROJDIR=/personal-projects/arduino/tofsensor
PORT=/dev/ttyACM0
#PORT=/dev/serial/by-id/usb-Raspberry_Pi_PicoArduino_E660382823419835-if00
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
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}

# Upload remote
ur: 
	scp pop-os:~${PROJDIR}/${IMAGE} /tmp
	sudo bossac -U --port=${PORT} -e -w /tmp/${IMAGE}

# Upload
#u: 
#	bossac --port=${PORT} -e -w -v -b ${IMAGE}

a:
	docker attach acli

s:
	screen ${PORT} 115200

u:
#	stty -F /dev/ttyACM0 1200
	bossac --port=${PORT}  -e -a  -w -v  ${BIN}

mc: 	# mount and copy
	./mntrp2040.sh
	sudo cp  ${UF2} ${MOUNT}

us:
	- bossac --port=${PORT} -w  ${BIN}
	./mntrp2040.sh
	sudo cp  ${UF2} ${MOUNT}
#	while [ ! -d "/media/mattd/RPI-RP2" ]; do echo -n "."; sleep 1; done
##	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
#	screen ${PORT} 115200
