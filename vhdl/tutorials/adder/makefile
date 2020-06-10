GHDL=~/bin/ghdl.sh
MODULE=adder_tb

all: compile elaborate run

compile: makefile $(MODULE).vhdl
	$(GHDL) -a $(MODULE).vhdl

elaborate: makefile 
	$(GHDL) -e $(MODULE)

run: makefile
	$(GHDL) -r $(MODULE) --wave=adder.wave

analyze: makefile
	$(GHDL) -a $(MODULE).vhdl

clean:
	rm *.cf
