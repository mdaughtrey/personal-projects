FQBN=arduino:avr:diecimila:cpu=atmega328
PORT=/dev/ttyUSB0
SKETCH=my


compile:
	arduino-cli compile --fqbn ${FQBN} ${SKETCH}

upload:
	arduino-cli upload -p ${PORT} --fqbn ${FQBN} ${SKETCH}
