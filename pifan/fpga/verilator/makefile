SHELL:=/bin/bash
TOP=top2.v
TAG=myspi

all: ${TAG}.v
	docker run -ti -v ${PWD}:/work --user $(shell id -u):$(shell id -g) verilator/verilator:latest --cc ${TAG}.v --exe sim_main.cpp

v:
	docker run -ti -v ${PWD}:/work --user $(shell id -u):$(shell id -g) verilator/verilator:latest --lint-only ${TAG}.v
run:
	obj_dir/V${TAG}

i: 
	docker run -v ${PWD}:/work -ti --entrypoint /bin/bash verilator/verilator:latest

build: sim_main.cpp
	make -j -C obj_dir V${TAG}.mk V${TAG}

#docker run -v ${PWD}:/work -ti --entrypoint /bin/bash verilator/verilator:latest
