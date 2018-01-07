PIO=platformio
ESP_NONOS=esp8266-nonos-sdk
ESP_RTOS=esp8266-rtos-sdk

all:
	${PIO} run

show:
	$(PIO) show $(ESP_NONOS)
	$(PIO) show $(ESP_RTOS)

up:
	sudo pio run -t upload --upload-port /dev/ttyS1 -v
