PIO=pio
BOARD=attiny85
UPLOAD_PORT=/dev/cu.usbserial-A6026P10
UPLOAD_SPEED=115200
#ESP_NONOS=esp8266-nonos-sdk
#ESP_RTOS=esp8266-rtos-sdk

all: 
	${PIO} run

clean:
	find . -name '*.o' -exec rm {} \;

#show:
#	$(PIO) show $(ESP_NONOS)
#	$(PIO) show $(ESP_RTOS)

u:
	pio run -t upload -v

init:
	pio init --board ${BOARD}

s:
	screen ${UPLOAD_PORT} ${UPLOAD_SPEED}
f:
	pio run -t fuses -v

test:
	avrdude -v -v -v -p attiny85 -C /Users/mattd/.platformio/packages/tool-avrdude/avrdude.conf -c buspirate -P/dev/cu.usbserial-A6026P10 -b115200 -b 115200 -e -D -U flash:w:.pioenvs/attiny85/firmware.hex:i
