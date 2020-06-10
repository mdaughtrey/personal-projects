PIO=platformio
PORT=/dev/cu.usbmodem141411
ESP_NONOS=esp8266-nonos-sdk
ESP_RTOS=esp8266-rtos-sdk

all:
	${PIO} run

#show:
#	$(PIO) show $(ESP_NONOS)
#	$(PIO) show $(ESP_RTOS)

up:
	pio run -t upload --upload-port $(PORT) -v

init:
	pio init --board genuino101

