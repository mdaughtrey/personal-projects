PIO=platformio
PORT=/dev/cu.SLAB_USBtoUART

all:
	${PIO} run

build:
	${PIO} run > build.log 2>&1

init:
	$(PIO) 
u:
	sudo pio run -t upload --upload-port $(PORT) -v 

s:
	screen $(PORT) 115200
