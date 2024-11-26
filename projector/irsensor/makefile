PROJ=irsensor
PROJDIR=/personal-projects/projector/irsensor
PORT=/dev/ttyACM0
#PORT=/dev/ttyS0
PLATFORM=rp2040:rp2040:generic
IMAGE=${PROJ}.ino.bin
UF2=${PROJ}.ino.uf2
ELF=${PROJ}.ino.elf
ACLI=/home/mattd/acli/bin/arduino-cli
DEV=/dev/sdb1
MOUNT=/media/rp2040

#DOCKERHOST=DESKTOP-TOJK787
DOCKERHOST=pop-os
DOCKERNAME=acli
DOCKER=docker

all:
	make b
#ssh mattd@${DOCKERHOST}  ${DOCKER} run --rm -v ~/dev/personal-projects:/personal-projects 
# Build remote
br:
	ssh mattd@${DOCKERHOST}  ${DOCKER} run --rm -v ~/personal-projects:/personal-projects  \
	--entrypoint /home/mattd/acli/bin/arduino-cli \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}
#	ssh mattd@pop-os  ${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
#        --entrypoint /home/mattd/acli/bin/arduino-cli \
#		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}
	
# Build
b:
	${DOCKER} run --rm -v ~/personal-projects:/personal-projects \
        --entrypoint /home/mattd/acli/bin/arduino-cli \
		--name ${DOCKERNAME} ${DOCKERNAME}:latest compile --clean ${PROJDIR}/${PROJ}.ino -b ${PLATFORM} --output-dir  ${PROJDIR}

# Upload remote
#ur: 
#	scp pop-os:~${PROJDIR}/${IMAGE} /tmp
#	sudo bossac -U --port=${PORT} -e -w /tmp/${IMAGE}

# Upload
u: 
	${ACLI} upload  -b ${PLATFORM} -p ${PORT} -i  ${UF2}
#	stty -F ${PORT} 1200
#	bossac --port=${PORT} -e -w -v -b ${IMAGE}
ur: 
	${ACLI} upload --log-level trace --log-file acli.log -b ${PLATFORM} -p ${PORT} -i /home/mattd/mnt/arduino/projv4/projv4.ino.uf2

a:
	docker attach acli

s:
	screen -L ${PORT} 115200

c:
	cat ${PORT} > serial.log

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

sshfs:
	sudo sshfs -o allow_other mattd@${DOCKERHOST}:/home/mattd/dev/personal-projects /home/mattd/mnt
	#sudo sshfs -o allow_other mattd@pop-os:/home/mattd/personal-projects /home/mattd/mnt

unmount:
	sudo fusermount -u /home/mattd/mnt

t:
	stty -F ${PORT} 1200

m:
	sudo mount ${DEV} ${MOUNT}

um:
	sudo umount ${MOUNT}

cp:
	sudo cp ${UF2} ${MOUNT}
