SHELL:=/bin/bash
TOP=top2.v
#SPISLAVE= 
#spi-slave-master/Verilog/source/SPI_Slave.v
#SPISLAVE=spi_verilog_master_slave_latest/spi_verilog_master_slave/trunk/rtl/spi_slave.v
SPISLAVE=myspi.v
SEG7=7seg.v
TIMER=timer.v
TEST=spitest.v

all:
	docker run -ti -v ${PWD}:/work --user $(shell id -u):$(shell id -g) verilator/verilator:latest --cc our.v --exe sim_main.cpp

run:
	obj_dir/Vour


i: 
	docker run -v ${PWD}:/work -ti --entrypoint /bin/bash verilator/verilator:latest

build:
	make -j -C obj_dir Vour.mk Vour
