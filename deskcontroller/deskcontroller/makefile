PROJ=deskcontroller
PORT=/dev/ttyUSB0
IMAGE=${PROJ}.ino.bin
ACLI=~/acli/bin/arduino-cli

b:
	${ACLI} compile ${PROJ}.ino -b esp8266:esp8266:generic --output-dir . 
	#${ACLI} compile ${PROJ}.ino -b esp8266:esp8266:generic --output-dir . --build-property build.extra_flags=-D__AVR__=1

u:
	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}

a:
	docker attach acli

s:
	screen ${PORT} 115200

us:
	esptool --port ${PORT} --chip esp8266 --baud 460800 write_flash --flash_size=detect 0 ${IMAGE}
	screen ${PORT} 115200
