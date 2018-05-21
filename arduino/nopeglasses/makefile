PIO=platformio
ESP_NONOS=esp8266-nonos-sdk
ESP_RTOS=esp8266-rtos-sdk
UPLOAD_PORT=/dev/cu.usbmodem14131
UPLOAD_SPEED=115200

all:
	${PIO} run

#show:
#	$(PIO) show $(ESP_NONOS)
#	$(PIO) show $(ESP_RTOS)

up:
	pio run -t upload --upload-port $(UPLOAD_PORT) -v

init:
	pio init --board genuino101

s:
	screen $(UPLOAD_PORT) 9600
