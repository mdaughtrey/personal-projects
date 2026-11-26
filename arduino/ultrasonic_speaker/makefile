PROJ=ultrasonic_speaker
PROJDIR=${PWD}
PORT=/dev/cu.usbserial-01EA920A
#PORT=/dev/ttyS0
#PLATFORM=rp2040:rp2040:generic
#PLATFORM=arduino:avr:micro
PLATFORM=esp32:esp32:esp32
IMAGE=${PROJ}.ino.bin
ACLI=arduino-cli

all: b u
	
# Build
b:
	${ACLI} compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}

# Container interactive
ci:
	${DOCKER} run -ti --rm --name ${DOCKERNAME} ${DOCKERNAME}:latest 
		
# Upload - remove and reinsert SS micro every  time
u:
	${ACLI} upload -i ${PROJDIR}/${IMAGE} -b ${PLATFORM} -p ${PORT}

d:
	${ACLI} debug --input-dir ~/${PROJDIR} -b ${PLATFORM} -p ${PORT}

ur: 
	${ACLI} upload --log-level trace --log-file acli.log -b ${PLATFORM} -p ${PORT} -i /home/mattd/mnt/arduino/projv4/projv4.ino.uf2

a:
	docker attach acli

s:
	screen -L ${PORT} 115200

c:
	cat ${PORT} > serial.log

ctags:
	ctags -R .  /Users/mattd/Documents/Arduino/libraries
