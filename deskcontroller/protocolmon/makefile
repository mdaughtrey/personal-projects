PROJ=protocolmon
PROJDIR=/personal-projects/deskcontroller/protocolmon
PORT=/dev/ttyUSB0
IMAGE=${PROJ}.ino.bin
ACLI=~/acli/bin/arduino-cli

DOCKERNAME=acli
DOCKER=docker

b:
	${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile ${PROJDIR}/${PROJ}.ino -b esp8266:esp8266:generic --output-dir  ${PROJDIR}
	#${ACLI} compile ${PROJ}.ino -b esp8266:esp8266:generic --output-dir . 
	#${ACLI} compile ${PROJ}.ino -b esp8266:esp8266:generic --output-dir . --build-property build.extra_flags=-D__AVR__=1

u:
	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ~${PROJDIR}/${IMAGE}

a:
	docker attach acli

s:
	screen ${PORT} 115200

c:
	cat ${PORT} > serial.log

#us:
#	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ~${PROJDIR}/${IMAGE}
#	screen ${PORT} 115200
