SHELL:=/bin/bash
TOP=top2.v
#SPISLAVE= 
#spi-slave-master/Verilog/source/SPI_Slave.v
#SPISLAVE=spi_verilog_master_slave_latest/spi_verilog_master_slave/trunk/rtl/spi_slave.v
SPISLAVE=myspi.v
SEG7=7seg.v
TIMER=timer.v

build:
	cat ${TOP} ${SPISLAVE} ${TIMER} ${SEG7} > main.v
	cp myspi.v /c/Users/matt/Downloads
	cp main.v /c/Users/matt/Downloads
	docker run -ti -v /c/Users/matt/Downloads:/work --user $(shell id -u):$(shell id -g) verilator/verilator:latest --cc main.v

io:
	cat iomon.v ${TIMER} ${SEG7} > main.v
	cp main.v /c/Users/matt/Downloads

verie:
	docker run -ti -v /c/Users/matt/Downloads:/work --entrypoint /bin/bash verilator/verilator:latest
