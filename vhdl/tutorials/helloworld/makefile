GHDL=~/bin/ghdl.sh

all: compile elaborate run

compile: makefile hello.vhdl
	$(GHDL) -a hello.vhdl

elaborate: makefile 
	$(GHDL) -e hello

run: makefile
	$(GHDL) -r hello

analyze: makefile
	$(GHDL) -a hello

clean:
	rm *.cf
