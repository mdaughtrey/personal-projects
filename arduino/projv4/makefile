PROJ=projv4
PROJDIR=/personal-projects/arduino/projv4
PORT=/dev/ttyACM0
PLATFORM=rp2040:rp2040:generic
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ACLI=~/acli/bin/arduino-cli

DOCKERNAME=acli
DOCKER=docker

# Build remote
br:
	ssh mattd@pop-os  ${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}
	
# Build
b:
	${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}

# Upload remote
ur: 
	scp pop-os:~${PROJDIR}/${IMAGE} /tmp
	sudo bossac -U --port=${PORT} -e -w /tmp/${IMAGE}

# Upload
u: 
	stty -F ${PORT} 1200
	bossac --port=${PORT} -e -w -v -b ${IMAGE}

a:
	docker attach acli

s:
	screen ${PORT} 115200

c:
	cat ${PORT} > serial.log

f:
	sudo scp mattd@pop-os:~${PROJDIR}/${UF2} /media/rp2040/

m:
	sudo mount /dev/sdc1 /media/rp2040  -o rw,users

#us:
#	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ~${PROJDIR}/${IMAGE}
#	screen ${PORT} 115200
